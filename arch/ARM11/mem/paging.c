#include "arch/ARM11/mem/sld.h"
#include "arch/ARM11/mem/fld.h"
#include "kernel/mem/paging.h"
#include "kernel/mem/mem.h"
#include "arch/ARM11/mem/mem.h"

uint32_t* free_slds[MEM_SLD_CACHE_SIZE];

void paging_create_default(void* loc, void* usr_loc)
{
	create_flat_fld0(usr_loc, 0, PLATFORM_PROC_MAX_MEM);
	if(PLATFORM_TTBCR_N!=0)
	{
		create_flat_fld1(loc, 0, PLATFORM_TOTAL_ADDR_RANGE);
#ifdef INIT_DEBUG
		uart_puts("Flat Page Tables set up\r\n");
#endif
	}
}

void mem_plat_init()
{
	for(uint32_t i=0; i<MEM_SLD_CACHE_SIZE; i++) free_slds[i] = 0;
}

uint32_t* get_free_sld()
{
	for(uint32_t i=0; i<MEM_SLD_CACHE_SIZE; i++)
	{
		//If there is a free sld to use
		if(free_slds[i])
		{
			return free_slds[i];
		}
	}
	#ifdef MEM_DBG_TBLS
		uart_puts("No free sld found\r\n");
	#endif
	uint32_t* addr = (uint32_t*) mem_phys_find_free(1<<12);
	//If we got an address we mark the region as used
	if(addr)
		mem_phys_set(addr, 1<<12);
	#ifdef MEM_DBG_TBLS
	uart_puts("New sld at ");
	uart_puthex((uint32_t)addr);
	uart_puts("\r\n");
	#endif
	return addr;
}

void free_sld(uint32_t* sld)
{
	for(uint32_t i=0; i<MEM_SLD_CACHE_SIZE; i++)
	{
		if(free_slds[i]==0)
		{
			free_slds[i] = sld;
			return;
		}
	}
	//If the buffer already contains the maximum amount of slds we want to cache
	//We just mark the physical memory as free
	mem_phys_clear(sld, 1<<12);
}

void mem_create_sld(uint32_t* fld_entry_addr, uint32_t offset, uint32_t len, void* phys_base, char domain, char perm, char caching, char global, char shared)
{
	uint32_t entry = *fld_entry_addr;
	#ifdef MEM_DBG_TBLS
	uart_puts("Creating sld at fld_entry ");
	uart_puthex((uint32_t)fld_entry_addr);
	uart_puts("\r\n");
	#endif
	//This routine should never be called on a pagetbl
	if(FLD_IS_PGTBL(entry))
		return;
	uint32_t s_index = offset>>12;
	uint32_t e_index = (offset+len-1)>>12;
	uint32_t p_base = (uint32_t)phys_base;
	
	#ifdef MEM_DBG_TBLS
	uart_puts("\ts_index ");
	uart_puthex(s_index);
	uart_puts("\r\n");
	uart_puts("\te_index ");
	uart_puthex(e_index);
	uart_puts("\r\n");
	#endif
	
	uint32_t* sld = get_free_sld();
	
	if(FLD_IS_SECTION(entry))
	{
		uint32_t s_base = (entry & FLD_SECTION_MASK);
		
		#ifdef MEM_DBG_TBLS
		uart_puts("FLD was section\r\n");
		uart_puthex((uint32_t)s_base);
		uart_puts("\r\n");
		uart_puthex(s_index);
		uart_puts("\r\n");
		uart_puthex(e_index);
		uart_puts("\r\n\r\n");
		#endif
		for(uint32_t i = 0; i<256; i++)
		{
			if(s_index<=i && i<=e_index)
				sld[i] = sld_create_small_entry((void*)(p_base+(i<<12)), perm, caching, global, shared);
			else
				sld[i] = sld_create_small_entry((void*)(s_base+(i<<12)), perm, caching, global, shared);
		}
		*fld_entry_addr = fld_construct_sld(sld, domain);
	}
	//If the section isn't mapped at all
	else
	{
		#ifdef MEM_DBG_TBLS
		uart_puts("FLD was unmapped\r\n");
		#endif
		for(uint32_t i = 0; i<256; i++)
		{
			if(s_index<=i && i<=e_index)
				sld[i] = sld_create_small_entry((void*)(p_base+(i<<12)), perm, caching, global, shared);
			else
				sld[i] = 0;
		}
		*fld_entry_addr = fld_construct_sld(sld, domain);
	}
}

void mem_map(uint32_t* fld_tbl, void* virt_addr, void* phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared)
{	
	uint32_t u_addr = (uint32_t) virt_addr;
	uint32_t e_addr = (u_addr + mem);
	uint32_t fld_start = u_addr >> 20;
	uint32_t fld_end =  (e_addr-1) >> 20;
	uint32_t p_addr = ((uint32_t)phys_addr) & (((1<<12)-1)<<20);
	
	for(uint32_t i = fld_start; i<=fld_end; i++, p_addr+=1<<20)
	{
		uint32_t fld_entry = fld_tbl[i];
		uint32_t sec_addr = i<<20;
			
		int32_t offset = u_addr-sec_addr;
		if(offset<0) offset = 0;
		
		int32_t len = e_addr-sec_addr;
		if(len>(1<<20)) len = 1<<20;
		len -= offset;
				
		//If we are mapping the whole MB
		if(offset == 0 && len == 1<<20)
		{
			if(FLD_IS_PGTBL(fld_entry))
				free_sld((uint32_t*)(fld_entry & FLD_PG_TBL_MASK));
			fld_tbl[i] = fld_construct_section((void*)p_addr, domain, 0, 0, 1, 0);
		}
		//otherwise we need to create a second level page table
		//if it was a section before we need to transfer all of those values over
		//if it was unmapped before we can just map the stuff we wanted before
		//if it's a page table we just set the corresponding values
		else
		{
			if(FLD_IS_PGTBL(fld_entry))
			{
				uint32_t* sld_tbl = (uint32_t*)(fld_entry&FLD_PG_TBL_MASK);	
				uint32_t sld_start = offset>>12;
				uint32_t sld_end = (offset+len-1)>>12;
			
				for(uint32_t j = sld_start; j<=sld_end; j++)
				{
					sld_tbl[j] = sld_create_small_entry((void*)(p_addr+(j<<12)), perm, caching, global, shared);
				}
			}
			else
			{
				mem_create_sld(fld_tbl+i, offset, len, (void*)p_addr, domain, perm, caching, global, shared);
			}
		}
	}
}

void mem_unmap(uint32_t* fld_tbl, void* virt_addr, size_t mem)
{	
	uint32_t u_addr = (uint32_t) virt_addr;
	uint32_t e_addr = (u_addr + mem);
	uint32_t fld_start = u_addr >> 20;
	uint32_t fld_end =  (e_addr-1) >> 20;
	
	for(uint32_t i = fld_start; i<=fld_end; i++)
	{
		uint32_t fld_entry = fld_tbl[i];
		uint32_t sec_addr = i<<20;
			
		int32_t offset = u_addr-sec_addr;
		if(offset<0) offset = 0;
		
		int32_t len = e_addr-sec_addr;
		if(len>(1<<20)) len = 1<<20;
		len -= offset;
		
		//If we are unmapping the whole MB
		if(offset == 0 && len == 1<<20)
		{
			if(FLD_IS_PGTBL(fld_entry))
				free_sld((uint32_t*)(fld_entry & FLD_PG_TBL_MASK));
			fld_tbl[i] = 0;
		}
		//otherwise we need to create a second level page table
		//if it was a section before we need to transfer all of those values over
		//if it is already a table we just set the values
		//If it isn't mapped there is nothing to unmap
		else
		{
			if(FLD_IS_PGTBL(fld_entry))
			{
				uint32_t* sld_tbl = (uint32_t*)(fld_entry&FLD_PG_TBL_MASK);	
				uint32_t sld_start = offset>>12;
				uint32_t sld_end = (offset+len-1)>>12;
			
				for(uint32_t j = sld_start; j<=sld_end; j++)
					sld_tbl[j] = 0;
			}
			else if(FLD_IS_SECTION(fld_entry))
			{
				mem_create_sld(fld_tbl+i, offset, len, (void*)0, 0, 0, 0, 0, 0);
				uint32_t* sld_tbl = (uint32_t*)((*(fld_tbl+i))&FLD_PG_TBL_MASK);
				uint32_t sld_start = offset>>12;
				uint32_t sld_end = (offset+len-1)>>12;
				#ifdef MEM_DBG_TBLS
				uart_puts("Entries SLD at ");
				uart_puthex((uint32_t)sld_tbl);
				uart_puts("\r\n");
				uart_puthex(sld_start);
				uart_puts("\r\n");
				uart_puthex(sld_end);
				uart_puts("\r\n\r\n");
				#endif
				for(uint32_t j = sld_start; j<=sld_end; j++)
					sld_tbl[j] = 0;
			}
		}
	}
}

void* mem_get_phys(uint32_t* fld_tbl, void* virt_addr)
{
	uint32_t u_addr = (uint32_t) virt_addr;
	uint32_t fld_index = u_addr >> 20; //how many MBs
	uint32_t entry = fld_tbl[fld_index];
	if(FLD_IS_SECTION(entry))
		return (void*)((entry & FLD_SECTION_MASK) | (u_addr&(~FLD_SECTION_MASK)));
	else if(FLD_IS_PGTBL(entry))
	{
		uint32_t* sld_tbl = (uint32_t*)(entry&FLD_PG_TBL_MASK);
		uint32_t sld_index = u_addr - (fld_index<<20); //Get offset inside 1MB
		sld_index >>= 12;							 //Get 4kb offset
		uint32_t sld_entry = sld_tbl[sld_index];
		//If the page is mapped as a small page
		if(sld_entry&0x2)
			return (void*)((sld_entry & SLD_SMALL_ADDR_MASK) | (u_addr & (~SLD_SMALL_ADDR_MASK)));
	}
	return (void*)0;
}