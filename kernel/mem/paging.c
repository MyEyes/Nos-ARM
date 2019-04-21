#include <stdio.h>
#include <string.h>

#include __PLATFORM__

#include "kernel/mem/paging.h"
#include "kernel/mem/perm.h"
#include "kernel/mem/mem.h"
#include "kernel/mem/cache.h"
#include "kernel/mem/cache.h"

//#define MEM_DBG_TBLS

pg_tbl_t kernel_page;
pg_tbl_t user_page;
uint32_t pg_initialized = 0;

pg_sld_t* free_slds[SLD_CACHE_SIZE];	//Cache for second level page descriptors that are unused

extern char __start;
extern char __text_end;

void pg_create_default(p_addr_t ker_loc, p_addr_t usr_loc)
{
	memset((char*)free_slds, 0, SLD_CACHE_SIZE*sizeof(pg_sld_t*));
	
	//Set up kernel pages
	pg_create(&kernel_page, ker_loc, (size_t)PLATFORM_TOTAL_ADDR_RANGE);
	printf("Kernel page (%x)->%x\r\n", &kernel_page, kernel_page.addr);
    pg_map_kernel();
	
	//Set up user pages
	pg_create(&user_page, usr_loc, (uint32_t)PLATFORM_PROC_MAX_MEM);
	pg_map(&user_page, 0, 0, (size_t)PLATFORM_PROC_MAX_MEM, 0, PERM_PRW_URW, 0, 1, 0);
	
	pg_initialized = 1;
}

void pg_map_kernel()
{
    //Identity map everything
	pg_map(&kernel_page, 0, 0, (size_t) PLATFORM_TOTAL_ADDR_RANGE, 0, PERM_PRW_UNA, 0, 1, 0);
    //Map higher half kernel start to 0 up to total phys memory
	pg_map(&kernel_page, (void*)((PLATFORM_KERNEL_VIRT_BASE_OFFSET)*BIG_PAGE_SIZE), 0, (uint32_t)PLATFORM_TOTAL_MEMORY, 0, PERM_PRW_UNA, 0, 1, 0);
    void* start = &__start;
    void* end = (&__text_end)-PLATFORM_KERNEL_BASE;
    printf("Making %x to %x read-only\r\n", start, end);
    //Map kernel code pages as read only
    pg_map(&kernel_page, (void*)((PLATFORM_KERNEL_VIRT_BASE_OFFSET*BIG_PAGE_SIZE)+start),start, (end-start), 0, PERM_PR_UNA, 0, 1, 0);
}

void pg_create(pg_tbl_t* tbl, p_addr_t entry_loc, size_t mem)
{
	tbl->addr = entry_loc;
	tbl->size = mem;
	uint32_t pages = mem/BIG_PAGE_SIZE;
    if(mem==(size_t)-1)
    {
        pages = 0x1000;
    }
	memset((char*)tbl->addr, 0, pages*sizeof(pg_fld_t));
}

uint32_t* pg_get_sld()
{
	for(uint32_t i=0; i<SLD_CACHE_SIZE; i++)
	{
		//If there is a free sld to use
		if(free_slds[i])
		{
			
			#ifdef MEM_DBG_TBLS
				printf("Found free sld: %x\r\n", free_slds[i]);
			#endif
			return free_slds[i];
		}
	}
	#ifdef MEM_DBG_TBLS
		printf("No free sld found\r\n");
	#endif
	uint32_t* addr = (uint32_t*) mem_phys_find_free(1<<12);
	//If we got an address we mark the region as used
	if(addr)
	{
		mem_phys_set(addr, SO_PAGES*sizeof(pg_sld_t));
		//Physical address might not be mapped
	}
	#ifdef MEM_DBG_TBLS
	printf("New sld at %x\r\n", addr);
	#endif
	return (uint32_t*)addr;
}

void pg_free_sld(uint32_t* sld)
{
	#ifdef MEM_DBG_TBLS
	printf("Sld at %x returned\r\n", sld);
	#endif
	for(uint32_t i=0; i<SLD_CACHE_SIZE; i++)
	{
		if(free_slds[i]==0)
		{
			free_slds[i] = sld;
			return;
		}
	}
	//If the buffer already contains the maximum amount of slds we want to cache
	//We just mark the physical memory as free
	mem_phys_clear(sld, SO_PAGES*sizeof(pg_sld_t));
}

void pg_free(pg_fld_t* fld)
{
	#ifdef PLATFORM_HAS_BIG_PAGES
	if(__plat_is_big_page(*fld))
		return;
	#endif
	//Free the sld that was used to map the memory of this fld
	pg_free_sld(__plat_sld_of_fld(*fld));
}

int pg_map_secondary(pg_fld_t* fld, v_addr_t virt_addr, p_addr_t phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared)
{
	pg_sld_t* sld = (pg_sld_t*)0;
	
	uint32_t v_addr = (uint32_t) virt_addr;
	
	#ifdef PLATFORM_HAS_BIG_PAGES
	if(__plat_is_big_page(*fld))
	{
		p_addr_t rphys;
		char rd;
		char rp;
		char rc;
		char rg;
		char rs;

		__plat_get_fld_parms(*fld, &rphys, &rd, &rp, &rc, &rg, &rs);

        #ifdef MEM_DBG_TBLS
        printf("Remapping FLD into SLDs!\r\nParams: phys:%x domain:%x permissions:%x cache:%x global:%x shared:%x\r\n",rphys, rd, rp, rc, rg, rs); 
        #endif

		*fld = 0;
        v_addr_t fld_vaddr = (v_addr_t)(v_addr&~(BIG_PAGE_SIZE-1));
		//Mapping from zero, cause we only care about the virtual address bits below PAGE_SIZE
		//And we want to map the whole page
		pg_map_secondary(fld, fld_vaddr, rphys, BIG_PAGE_SIZE, rd, rp, rc, rg, rs);
		//After return, *fld will be set to be an sld page
	}
	#endif
	
	//If the page isn't mapped yet we just map it
	if(!*fld)
	{
		sld = pg_get_sld();
		//If we can't allocate a second level descriptor we error out
		if(!sld)
			return -1;
		*fld = __plat_fld_create(sld, domain, perm, caching, global, shared);
	}
	else
	{
		sld = __plat_sld_of_fld(*fld);
		//If fetching the sld failed we error out
		if(!sld)
			return -1;
	}
	sld = (pg_sld_t*)TO_KERNEL_ADDR_SPACE(sld);
	#ifdef MEM_DBG_TBLS
	printf("v_addr: %x, mem: %x\r\n", v_addr, mem);
	#endif
	//Find out the offsets inside the slds that we need to map
	uint32_t start = (v_addr & (BIG_PAGE_SIZE - 1))/PAGE_SIZE;
	uint32_t num = ((mem+PAGE_SIZE-1))/PAGE_SIZE;
	if(num+start>SO_PAGES)
		num = SO_PAGES-start;
	#ifdef MEM_DBG_TBLS
	printf("Mapping to %x\r\n", phys_addr);
	printf("Start: %x, Num: %x\r\n", start, num);
	#endif
	for(uint32_t i = start; i<start+num; i++)
    {
		sld[i] = __plat_sld_create(phys_addr + (i-start)*PAGE_SIZE, domain, perm, caching, global, shared);
    }
    data_cache_clean();
	return 0;
}

int pg_unmap_secondary(pg_fld_t* fld, v_addr_t virt_addr, size_t mem)
{
	pg_sld_t* sld = (pg_sld_t*)0;
	uint32_t v_addr = (uint32_t) virt_addr;
	
	#ifdef PLATFORM_HAS_BIG_PAGES
	if(__plat_is_big_page(*fld))
	{
		p_addr_t rphys;
		char rd;
		char rp;
		char rc;
		char rg;
		char rs;
		__plat_get_fld_parms(*fld, &rphys, &rd, &rp, &rc, &rg, &rs);
		*fld = 0;
		//Mapping from zero, cause we only care about the virtual address bits below PAGE_SIZE
		//And we want to map the whole page
		pg_map_secondary(fld, 0, rphys, BIG_PAGE_SIZE, rd, rp, rc, rg, rs);
		//After return, *fld will be set to be an sld page
	}
	#endif
	
	//If the page isn't mapped yet we're done
	if(!*fld)
	{
		return 0;
	}
	else
	{
		sld = __plat_sld_of_fld(*fld);
		//If fetching the sld failed we error out
		if(!sld)
			return -1;
	}
	
	//Find out the offsets inside the slds that we need to map
	uint32_t start = (v_addr & ~(PAGE_SIZE - 1))/PAGE_SIZE;
	uint32_t end = ((v_addr+mem+PAGE_SIZE-1) & ~(PAGE_SIZE - 1))/PAGE_SIZE;
	
	for(uint32_t i = start; i<end; i++)
		sld[i] = 0;
    data_cache_clean();
	return 0;
}

int pg_map(pg_tbl_t* tbl, v_addr_t virt_addr, p_addr_t phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared)
{	
    //printf("Trying to map %x to %x(%x)", virt_addr, phys_addr, mem);
	if((uint32_t)virt_addr+mem > tbl->size && tbl->size){
        printf(" FAIL!\r\n");
		return -1;
    }
	//printf(" OK!\r\n");
	pg_fld_t* flds = tbl->addr;
	//fld_t	|fld_t|fld_t
	//MEM	|sld_t
	
	size_t u_addr = (size_t) virt_addr;				//virtual address start cast as uint
	size_t e_addr = (u_addr + mem);					//virtual address end
	size_t fld_start = u_addr / BIG_PAGE_SIZE;		//starting first level descriptor
	size_t fld_end =  (e_addr-1) / BIG_PAGE_SIZE;		//last first level descriptor
	
	size_t start_virt = fld_start*BIG_PAGE_SIZE;						//starting v_address of current descriptor
	size_t p_address = ((size_t)phys_addr) & ~(PAGE_SIZE-1);	//big page aligned physical address
	
	//Be ready to modify all the first level descriptors we need to touch
	for(uint32_t i = fld_start; i<=fld_end; i++, p_address+=BIG_PAGE_SIZE, start_virt+=BIG_PAGE_SIZE)
	{
		#ifdef PLATFORM_HAS_BIG_PAGES
		//If we are not mapping the full page
		if(start_virt<u_addr || e_addr<start_virt+BIG_PAGE_SIZE)
		{
		#endif
			//Figure out the correct start and end address
			v_addr_t s = (v_addr_t) start_virt;
			v_addr_t e = (v_addr_t) start_virt + BIG_PAGE_SIZE;
			if((size_t)s < u_addr)
			{
				s = (v_addr_t)u_addr;
			}
			if(e_addr < (size_t)e || !e)
				e = (v_addr_t)e_addr;
			//Map a secondary page table with the correct start and end address
			#ifdef MEM_DBG_TBLS
			printf("Mapping secondary page: %x -> %x\r\n", s, p_address);
			printf("Range: %x-%x:%x\r\n", s, e, e-s);
			#endif
			int ret = pg_map_secondary(flds + i, s, (p_addr_t)p_address, e-s, domain, perm, caching, global, shared);
			//If mapping any page here fails we return -1
			if(ret == -1)
				return -1;
		#ifdef PLATFORM_HAS_BIG_PAGES
		}
		else
		{
			//If this page was mapped we need to clean it up
			if(flds[i])
				pg_free(flds + i);
			//otherwise map the whole large page
			flds[i] = __plat_fld_big((p_addr_t)p_address, domain, perm, caching, global, shared);
		}
		#endif
	}
    data_cache_clean();
	return 0;
}

int pg_unmap(pg_tbl_t* tbl, v_addr_t virt_addr, size_t mem)
{	
	if((uint32_t)virt_addr+mem > tbl->size && tbl->size)
		return -1;
	
	pg_fld_t* flds = tbl->addr;
	
	size_t u_addr = (size_t) virt_addr;				//virtual address start cast as uint
	size_t e_addr = (u_addr + mem);					//virtual address end
	size_t fld_start = u_addr / BIG_PAGE_SIZE;		//starting first level descriptor
	size_t fld_end =  (e_addr-1) / BIG_PAGE_SIZE;		//last first level descriptor
	
	size_t start_virt = fld_start*BIG_PAGE_SIZE;						//starting v_address of current descriptor
	
	//Be ready to modify all the first level descriptors we need to touch
	for(uint32_t i = fld_start; i<=fld_end; i++, start_virt+=BIG_PAGE_SIZE)
	{
		#ifdef PLATFORM_HAS_BIG_PAGES
		//If we are not mapping the full page
		if(start_virt<u_addr || e_addr<start_virt+BIG_PAGE_SIZE)
		{
		#endif
			//Figure out the correct start and end address
			v_addr_t s = (v_addr_t) start_virt;
			v_addr_t e = (v_addr_t) start_virt + BIG_PAGE_SIZE;
			if((size_t)s < u_addr)
				s = (v_addr_t)u_addr;
			if(e_addr < (size_t)e)
				e = (v_addr_t)e_addr;
			//unmap a secondary page table with the correct start and end address
			int ret = pg_unmap_secondary(flds + i, s,  e-s);
			//If mapping any page here fails we return -1
			if(ret == -1)
				return -1;
		#ifdef PLATFORM_HAS_BIG_PAGES
		}
		else
		{
			//If this page was mapped we need to clean it up
			if(flds[i])
				pg_free(flds + i);
			//otherwise unmap the whole large page
			flds[i] = 0;
		}
		#endif
	}
    mem_dmb();
    mem_dsb();
    data_cache_clean();
	return 0;
}

void* pg_get_phys(pg_tbl_t* tbl, v_addr_t virt_addr)
{
	return __plat_pg_get_phys(tbl->addr, virt_addr);
}

uint32_t pg_get_entry(pg_tbl_t* tbl, v_addr_t virt_addr)
{
	return __plat_pg_get_entry(tbl->addr, virt_addr);
}

#ifdef MEM_DBG_TBLS
void pg_fld_sld_dbg(pg_tbl_t* tbl, v_addr_t virt)
{
    uint32_t virt_low = (uint32_t)virt & ~(BIG_PAGE_SIZE-1 );
    pg_fld_t fld = tbl->addr[virt_low/BIG_PAGE_SIZE];
    pg_sld_t* slds = __plat_sld_of_fld(fld);
    printf("fld: %x\r\n", fld);
    printf("sld at %x\r\n", slds);
    for(uint32_t x=0; x<256; x++)
    {
        uint32_t sld = pg_get_entry(tbl, (void*)virt_low+x*PAGE_SIZE);
        printf("sld: %x %x\r\n", x, sld);
    }
}
#endif
