#ifndef __PAGING_H__
#define __PAGING_H__

#include __PLATFORM__

#define KERNEL_DEF_PG_LOC PLATFORM_PROC_MAX_MEM
#define KERNEL_DEF_USR_PG_LOC (KERNEL_DEF_PG_LOC+0x100000)

void paging_create_default(void* loc, void* usr_loc);
void paging_create_flat(void* loc, void* phys_start, void* phys_end);

#endif