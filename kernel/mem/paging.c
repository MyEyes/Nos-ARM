#include <stdio.h>
#include <string.h>

#include __PLATFORM__

#include "kernel/mem/paging.h"
#include "kernel/mem/perm.h"
#include "kernel/mem/mem.h"

pg_tbl_t kernel_page;
pg_tbl_t user_page;
uint32_t pg_initialized = 0;

pg_so_desc_t* free_so_descs[SO_CACHE_SIZE];	//Cache for second level page descriptors that are unused

void pg_create_default(p_addr_t ker_loc, p_addr_t usr_loc)
{
	uint32_t ent_size = BIG_PAGE_SIZE;
	
	//Set up kernel pages
	pg_create(&kernel_page, ker_loc, (size_t)PLATFORM_TOTAL_ADDR_RANGE);
	
	pg_map(&kernel_page, 0, 0, ((PLATFORM_KERNEL_VIRT_BASE_OFFSET)*ent_size), 0, PERM_PRW_URW, 0, 1, 0);
	pg_map(&kernel_page, (void*)((PLATFORM_KERNEL_VIRT_BASE_OFFSET)*ent_size), 0, (uint32_t)PLATFORM_TOTAL_MEMORY, 0, PERM_PRW_UNA, 0, 1, 0);
	pg_map(&kernel_page,(void*)((PLATFORM_KERNEL_VIRT_BASE_OFFSET)*ent_size+(uint32_t)PLATFORM_TOTAL_MEMORY), (void*)((PLATFORM_KERNEL_VIRT_BASE_OFFSET)*ent_size+(uint32_t)PLATFORM_TOTAL_MEMORY), (uint32_t)PLATFORM_TOTAL_ADDR_RANGE-((PLATFORM_KERNEL_VIRT_BASE_OFFSET)*ent_size+(uint32_t)PLATFORM_TOTAL_MEMORY), 0, PERM_PRW_URW, 0, 1, 0);
	
	//Set up user pages
	pg_create(&user_page, usr_loc, (uint32_t)PLATFORM_PROC_MAX_MEM);
	pg_map(&user_page, 0, 0, (size_t)PLATFORM_PROC_MAX_MEM, 0, PERM_PRW_URW, 0, 1, 0);
	
	//Clear cache of so descs
	memset((char*)free_so_descs, 0, SO_CACHE_SIZE*sizeof(pg_so_desc_t*));
	
	pg_initialized = 1;
}

void pg_create(pg_tbl_t* tbl, p_addr_t entry_loc, size_t mem)
{
	tbl->addr = entry_loc;
	tbl->size = mem;
	uint32_t pages = mem/BIG_PAGE_SIZE;
	memset((char*)tbl->addr, 0, pages*sizeof(pg_fo_desc_t));
}

void pg_map(pg_tbl_t* tbl, v_addr_t virt_addr, p_addr_t phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared)
{
	
	if((uint32_t)virt_addr+mem <= tbl->size || !tbl->size)
		__plat_pg_map(tbl->addr, virt_addr, phys_addr, mem, domain, perm, caching, global, shared);
}

void pg_unmap(pg_tbl_t* tbl, v_addr_t virt_addr, size_t mem)
{
	if((uint32_t)virt_addr+mem <= tbl->size || !tbl->size)
		__plat_pg_unmap(tbl->addr, virt_addr, mem);
}

void* pg_get_phys(pg_tbl_t* tbl, v_addr_t virt_addr)
{
	return __plat_pg_get_phys(tbl->addr, virt_addr);
}

uint32_t pg_get_entry(pg_tbl_t* tbl, v_addr_t virt_addr)
{
	return __plat_pg_get_entry(tbl->addr, virt_addr);
}