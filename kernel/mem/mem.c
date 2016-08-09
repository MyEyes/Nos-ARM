#include <stdint.h>
#include "kernel/mem/mem.h"
#include "kernel/mem/fld.h"
#include "kernel/mem/sld.h"
#ifdef MEM_DBG_TBLS
#include "kernel/util/kernel_uart.h"
#endif

MB_phys_desc_t* desc_tbl = 0;
size_t num_descs = 0;

uint32_t* free_slds[MEM_SLD_CACHE_SIZE];

uint32_t mem_phys_free = (uint32_t)PLATFORM_TOTAL_MEMORY;
uint32_t mem_phys_used = 0;

void mem_phys_init(size_t total_mem)
{
	desc_tbl = KERNEL_PHYS_MEM_TBL;
	num_descs = total_mem >> 20; //number of entries, if there is a partial MB somehow, we just throw it away
	for(uint32_t i=0; i<MEM_SLD_CACHE_SIZE; i++) free_slds[i] = 0;
} 

void mem_phys_set(void* phys_addr, size_t mem)
{
	mem_phys_set_val(phys_addr, mem, 1);
	mem_phys_free -= mem;
	mem_phys_used += mem;
}

void mem_phys_clear(void* phys_addr, size_t mem)
{
	mem_phys_set_val(phys_addr, mem, 0);
	mem_phys_used -= mem;
	mem_phys_free += mem;
}


void mem_phys_set_val(void* phys_addr, size_t mem, char val)
{
	uint32_t phys_addr_u = (uint32_t) phys_addr;
	uint32_t phys_addr_last = ((uint32_t) phys_addr) + mem;
	uint32_t index = (phys_addr_u>>20);
	uint32_t endindex = (phys_addr_last>>20);
	
	for(; index<=endindex; index++)
	{
		MB_phys_desc_t* curr_desc = desc_tbl + index;
		//figure out how much we still have to set
		uint32_t curr_len = phys_addr_last - phys_addr_u;

		uint32_t curr_offset = phys_addr_u-(index<<20);
		//if it's more than 1MB, round down to 1MB
		if(curr_len > (1<<20))
		{
			curr_len = 1<<20;
			curr_len -= curr_offset;
		}
		if(val)
			mem_phys_desc_set(curr_desc, curr_offset, curr_len);
		else
			mem_phys_desc_clear(curr_desc, curr_offset, curr_len);
		
		phys_addr_u = (index+1)<<20;
	}
}

void* mem_phys_find_free(size_t mem)
{
	//If we are asking for over 1MB return the null pointer
	//the point is that we will only return one address and finding several MB of contiguous memory seems unlikely
	if(mem > (1<<20))
		return 0;
	else if(mem==0)
		return 0;
	else
	{
		uint32_t curr_len = 256; //Number of 4kb entries
		uint32_t curr_offset = 256; //Offset of first 4kb entry from struct start in bits (32*8)
		uint32_t curr_size = 0x1000; //4kb
		mem -= 1;
		mem >>= 12; //Round mem down to 4kb boundary
		//Since we don't have a log function we can just loop this
		//to figure out how much we need to allocate and what resolution to look at.
		while(mem>0)
		{
			mem >>= 1;
			curr_len >>= 1;
			curr_offset >>= 1;
			curr_size <<= 1;
		}
		
		for(uint32_t curr_desc = 0; curr_desc<num_descs; curr_desc++)
		{
			for(uint32_t curr_entry = 0; curr_entry<curr_len; curr_entry++)
				if(!mem_bit_set((char*)(desc_tbl+curr_desc), curr_offset+curr_entry))
					return (void*)((curr_desc<<20)+curr_size*curr_entry);
		}
		//If we can't find an entry that fits, we do this
		return 0;
	}
}

void mem_phys_desc_set(MB_phys_desc_t* desc, uint32_t offset, size_t mem)
{
	if(mem==0)
		return;
	
	uint32_t curr_offset = offset>>12; //(shift right to get alignment to 4kb)
	uint32_t end_offset = (offset+mem-1)>>12;
		
	char* desc_c = (char*)desc;
	
	//Step is the offset from the bitmaps start in bits for each resolution
	//So we potentially touch any bit but the first
	for(uint32_t step = 32*8; step>=1; step>>=1)
	{
		char* curr = desc_c;
		mem_set_bits(curr, curr_offset+step, end_offset+step);
		curr_offset >>= 1;
		end_offset >>= 1;
	}
	
}

void mem_phys_desc_clear(MB_phys_desc_t* desc, uint32_t offset, size_t mem)
{
	if(mem==0)
		return;
		
	int32_t curr_offset = offset>>12; //(shift right to get alignment to 4kb)
	int32_t end_offset = (offset+mem-1)>>12;
		
	char* desc_c = (char*)desc;
	//Step is the offset from the bitmaps start in bits for each resolution
	//So we potentially touch any bit but the first
	for(uint32_t step = 32*8; step>=1; step>>=1)
	{
		char* curr = desc_c;
		mem_clear_bits(curr, curr_offset + step, end_offset + step);
		//If we are at an odd position and the previous bit is not clear, we can not set the top level of that as clear
		if(curr_offset&0x1)
			if(mem_bit_set(curr, curr_offset + step - 1))
				curr_offset += 1;
		//If we end at an even position and the following bit is not clear, we can not set the top level of that as clear
		if(!(end_offset&0x1))
			if(mem_bit_set(curr, end_offset + step + 1))
				end_offset -= 1;
		//The only way this can happen is if they have the same value but we can't set the level above it
		//So if this occurs, we stop clearing bits
		if(curr_offset>end_offset)
			break;
		curr_offset >>= 1;
		end_offset >>= 1;
	}
}

void mem_mark_as_kernel(void* phys_addr, size_t mem)
{
	uint32_t index = ((uint32_t)phys_addr)>>20;
	uint32_t end_index = ((uint32_t)phys_addr+mem)>>20;
	for(;index<=end_index; index++)
		desc_tbl[index].large_chunks |= 1;
}

void mem_unmark_as_kernel(void* phys_addr, size_t mem)
{
	uint32_t index = ((uint32_t)phys_addr)>>20;
	uint32_t end_index = ((uint32_t)phys_addr+mem)>>20;
	for(;index<=end_index; index++)
		desc_tbl[index].large_chunks &= 0xFE;
}

void mem_set_bits(char* base, uint32_t start_bit, uint32_t end_bit)
{
	char s_bit_under = start_bit & 0x7; 
	char e_bit_over = ((end_bit+1) & 0x7); 
	
	uint32_t start = (start_bit + 7)>>3; 
	uint32_t end = (end_bit+1)>>3; 
	
	//If bits are not byte aligned for the start
	if((start_bit>>3)==end)
		base[start_bit>>3] |= ((1 << (e_bit_over - s_bit_under))-1) << s_bit_under;
	else
	{
		if(s_bit_under)
			base[start_bit>>3] |= ((unsigned char)-1) << s_bit_under;
		if(e_bit_over)
			base[end] |= (1<<e_bit_over)-1;
	}
	
	for(uint32_t c = start; c<end; c++)
		base[c] = (unsigned char)-1;
}

void mem_clear_bits(char* base, uint32_t start_bit, uint32_t end_bit)
{
	char s_bit_under = start_bit & 0x7; 
	char e_bit_over = ((end_bit+1) & 0x7);
	
	uint32_t start = (start_bit + 7)>>3;
	uint32_t end = (end_bit+1)>>3;
		
	//If bits are not byte aligned for the start
	if((start_bit>>3)==end)
		base[start_bit>>3] &= ~(((1 << (e_bit_over - s_bit_under))-1) << s_bit_under);
	else
	{
		if(s_bit_under)
			base[start_bit>>3] &= ~(((unsigned char)-1) << s_bit_under);
		if(e_bit_over)
			base[end] &= ~((1<<e_bit_over)-1);
	}
	
	for(uint32_t c = start; c<end; c++)
		base[c] = (unsigned char)0;
}

char mem_bit_set(char* base, uint32_t bit)
{
	uint32_t byte = bit >> 3;
	bit &= 7;
	return (base[byte]>>bit) & 1;
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