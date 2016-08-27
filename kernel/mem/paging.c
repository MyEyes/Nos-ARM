#include <stdio.h>
#include <string.h>

#include __PLATFORM__

#include "kernel/mem/paging.h"
#include "kernel/mem/perm.h"
#include "kernel/mem/mem.h"

pg_tbl_t kernel_page;
pg_tbl_t user_page;
uint32_t pg_initialized = 0;

void pg_create_default(void* ker_loc, void* usr_loc)
{
	uint32_t ent_size = __plat_pg_tbl_maxentry();
	
	kernel_page.addr = ker_loc;
	kernel_page.size = (uint32_t)PLATFORM_TOTAL_ADDR_RANGE;
	__plat_pg_clear(ker_loc, (size_t)PLATFORM_TOTAL_ADDR_RANGE);
	__plat_pg_map(ker_loc, 0, 0, ((PLATFORM_KERNEL_VIRT_BASE_OFFSET)*ent_size), 0, PERM_PRW_URW, 0, 1, 0);
	__plat_pg_map(ker_loc, (void*)((PLATFORM_KERNEL_VIRT_BASE_OFFSET)*ent_size), 0, (uint32_t)PLATFORM_TOTAL_MEMORY, 0, PERM_PRW_UNA, 0, 1, 0);
	__plat_pg_map(ker_loc,(void*)((PLATFORM_KERNEL_VIRT_BASE_OFFSET)*ent_size+(uint32_t)PLATFORM_TOTAL_MEMORY), (void*)((PLATFORM_KERNEL_VIRT_BASE_OFFSET)*ent_size+(uint32_t)PLATFORM_TOTAL_MEMORY), (uint32_t)PLATFORM_TOTAL_ADDR_RANGE-((PLATFORM_KERNEL_VIRT_BASE_OFFSET)*ent_size+(uint32_t)PLATFORM_TOTAL_MEMORY), 0, PERM_PRW_URW, 0, 1, 0);
	
	user_page.addr = usr_loc;
	user_page.size = (uint32_t)PLATFORM_PROC_MAX_MEM;
	
	uint32_t num_user_entries = (uint32_t)PLATFORM_PROC_MAX_MEM/ent_size;
	
	__plat_pg_clear(usr_loc, (size_t)PLATFORM_PROC_MAX_MEM);
	__plat_pg_map(usr_loc, 0, 0, num_user_entries*ent_size,0, PERM_PRW_URW, 0, 1, 0);
	pg_initialized = 1;
}

void pg_create(pg_tbl_t* tbl, void* entry_loc, size_t mem)
{
	tbl->addr = entry_loc;
	tbl->size = mem;
}

void pg_map(pg_tbl_t* tbl, void* virt_addr, void* phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared)
{
	if((uint32_t)virt_addr+mem <= tbl->size || !tbl->size)
		__plat_pg_map(tbl->addr, virt_addr, phys_addr, mem, domain, perm, caching, global, shared);
}

void pg_unmap(pg_tbl_t* tbl, void* virt_addr, size_t mem)
{
	if((uint32_t)virt_addr+mem <= tbl->size || !tbl->size)
		__plat_pg_unmap(tbl->addr, virt_addr, mem);
}

void* pg_get_phys(pg_tbl_t* tbl, void* virt_addr)
{
	return __plat_pg_get_phys(tbl->addr, virt_addr);
}

uint32_t pg_get_entry(pg_tbl_t* tbl, void* virt_addr)
{
	return __plat_pg_get_entry(tbl->addr, virt_addr);
}