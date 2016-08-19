#include "kernel/mem/paging.h"
#include "kernel/mem/perm.h"
#include "kernel/mod/kernel_uart.h"
#include __PLATFORM__

pg_tbl_t kernel_page;
pg_tbl_t user_page;

void pg_create_default(void* ker_loc, void* usr_loc)
{
	uint32_t ent_size = __plat_pg_tbl_maxentry();
	
	kernel_page.addr = ker_loc;
	kernel_page.size = (uint32_t)PLATFORM_TOTAL_ADDR_RANGE;
	
	uint32_t num_ker_entries = (uint32_t)PLATFORM_TOTAL_ADDR_RANGE/ent_size;
	for(uint32_t i=0; i<num_ker_entries; i++)
		if(i<PLATFORM_KERNEL_VIRT_BASE_OFFSET || i>=PLATFORM_KERNEL_VIRT_BASE_OFFSET+PLATFORM_KERNEL_VIRT_SIZE)
			__plat_pg_map(ker_loc, (void*)(i * ent_size), (void*)(i * ent_size), ent_size, 0, PERM_PRW_URW, 0, 1, 0);
		else
			__plat_pg_map(ker_loc, (void*)(i * ent_size), (void*)((i-PLATFORM_KERNEL_VIRT_BASE_OFFSET) * ent_size), ent_size, 0, PERM_PRW_UNA, 0, 1, 0);
	
	user_page.addr = usr_loc;
	user_page.size = (uint32_t)PLATFORM_PROC_MAX_MEM;
	
	uint32_t num_user_entries = (uint32_t)PLATFORM_PROC_MAX_MEM/ent_size;
	
	for(uint32_t i=0; i<num_user_entries; i++)
		if(i<PLATFORM_KERNEL_VIRT_BASE_OFFSET || i>=PLATFORM_KERNEL_VIRT_BASE_OFFSET+PLATFORM_KERNEL_VIRT_SIZE)
			__plat_pg_map(usr_loc, (void*)(i * ent_size), (void*)(i * ent_size), ent_size, 0, PERM_PRW_URW, 0, 1, 0);
		else //Do not map kernel memory into a user entry
			__plat_pg_unmap(usr_loc, (void*)(i * ent_size), ent_size);
}

void pg_map(pg_tbl_t* tbl, void* virt_addr, void* phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared)
{
	if((uint32_t)virt_addr+mem < tbl->size)
		__plat_pg_map(tbl->addr, virt_addr, phys_addr, mem, domain, perm, caching, global, shared);
}

void pg_unmap(pg_tbl_t* tbl, void* virt_addr, size_t mem)
{
	if((uint32_t)virt_addr+mem < tbl->size)
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