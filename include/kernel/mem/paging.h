#ifndef __PAGING_H__
#define __PAGING_H__

#include __PLATFORM__
#include <stdint.h>

#define KERNEL_DEF_PG_LOC PLATFORM_PROC_MAX_MEM
#define KERNEL_DEF_USR_PG_LOC (KERNEL_DEF_PG_LOC+0x100000)

typedef struct
{
	void* 		addr;
	uint32_t	size;
} pg_tbl_t;

void paging_create_default(void* loc, void* usr_loc);
void paging_create_flat(void* loc, void* phys_start, void* phys_end);

void pg_map(pg_tbl_t* tbl, void* virt_addr, void* phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared);
void pg_unmap(pg_tbl_t* tbl, void* virt_addr, size_t mem);
void* pg_get_phys(pg_tbl_t* tbl, void* virt_addr);
uint32_t pg_get_entry(pg_tbl_t* tbl, void* virt_addr);

#endif