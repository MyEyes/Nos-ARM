#include "kernel/proc/syscall.h"
#include "kernel/proc/thread.h"
#include "kernel/mem/paging.h"
#include "kernel/mem/perm.h"
#include "kernel/hw/devmap.h"
#include "kernel/mem/mmu.h"

#include <string.h>
#include <sys/types.h>
#include <stdio.h>

void *syscall_tbl[SYSCALL_TBL_MAX];

void syscall_init()
{
	memset((char*)syscall_tbl, 0, sizeof(void*)*SYSCALL_TBL_MAX);
	
	syscall_set(SYSCALL_PROC_SBRK, sbrk);
	syscall_set(SYSCALL_PROC_FORK, fork);
    syscall_set(SYSCALL_PMAP, sys_pmap);
}

void syscall_set(syscall_id_t id, void* trg)
{
	if(id<SYSCALL_TBL_MAX)
		syscall_tbl[id] = trg;
}

void* sys_pmap()
{
	printf("Doing pmap!\r\n");
    p_addr_t phys_addr = (p_addr_t)__plat_thread_getparam(curr_thread, 1);
    v_addr_t virt = (v_addr_t)__plat_thread_getparam(curr_thread, 2);
    size_t mem = (size_t) __plat_thread_getparam(curr_thread, 3);

    //Align allocated memory to pagesize
	mem = (mem+PAGE_SIZE-1)&~(PAGE_SIZE-1);
    //Check that target virtual address is aligned
    if((uint32_t)virt&(PAGE_SIZE-1))
        return (void*)-1;
    //Check that target virtual address is in reserved region
    if((uint32_t)virt<PROC_KRES_LOC || (uint32_t)virt>=PROC_KRES_END)
        return (void*)-1;

    pg_map(curr_thread->proc->pg_tbl, (void*)virt,  phys_addr, mem, 0, curr_thread->proc->priv?PERM_PRW_UNA:PERM_PRW_URW, 0, 0, 0);

//    pg_fld_sld_dbg(curr_thread->proc->pg_tbl, virt);

    return (void*)virt;
}

