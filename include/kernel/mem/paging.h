#ifndef __PAGING_H__
#define __PAGING_H__

#include __PLATFORM__
#include <stdint.h>
#include <stddef.h>

#define KERNEL_DEF_PG_LOC PLATFORM_PROC_MAX_MEM
#define KERNEL_DEF_USR_PG_LOC ((char*)KERNEL_DEF_PG_LOC+0x100000)

#define TO_KERNEL_ADDR_SPACE(u) ((uint32_t)u<(uint32_t)PLATFORM_KERNEL_BASE?(uint32_t)u+PLATFORM_KERNEL_BASE:(uint32_t)u)

typedef struct
{
	void* 		addr;
	uint32_t	size;
} pg_tbl_t;

extern pg_tbl_t kernel_page;
extern pg_tbl_t user_page;

void pg_create_default(void* loc, void* usr_loc);

void pg_create(pg_tbl_t* tbl, void* entry_loc, size_t mem);

void pg_map(pg_tbl_t* tbl, void* virt_addr, void* phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared);
void pg_unmap(pg_tbl_t* tbl, void* virt_addr, size_t mem);
void* pg_get_phys(pg_tbl_t* tbl, void* virt_addr);
uint32_t pg_get_entry(pg_tbl_t* tbl, void* virt_addr);

void __plat_pg_map(void* tbl_addr, void* virt_addr, void* phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared);
void __plat_pg_unmap(void* tbl_addr, void* virt_addr, size_t mem);
void* __plat_pg_get_phys(void* tbl_addr, void* virt_addr);
void* __plat_pg_get_kern(void* tbl_addr, void* virt_addr);
uint32_t __plat_pg_get_entry(void* tbl_addr, void* virt_addr);
uint32_t __plat_pg_tbl_maxentry();
void __plat_pg_clear(void* tbl_addr, size_t mem);

#endif