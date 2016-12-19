#include "kernel/proc/syscall.h"
#include "kernel/proc/thread.h"
#include "kernel/mem/paging.h"
#include "kernel/mem/perm.h"
#include "kernel/hw/devmap.h"

#include <string.h>
#include <sys/types.h>
#include <stdio.h>

void *syscall_tbl[SYSCALL_TBL_MAX];

void syscall_init()
{
	memset((char*)syscall_tbl, 0, sizeof(void*)*SYSCALL_TBL_MAX);
	
	syscall_set(SYSCALL_PROC_SBRK, sbrk);
	syscall_set(SYSCALL_PROC_FORK, fork);
	syscall_set(SYSCALL_DEV_RQ, sys_dev_rq);
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

    pg_map(curr_thread->proc->pg_tbl, (void*)virt,  phys_addr, mem, 0, curr_thread->proc->priv?PERM_PRW_UNA:PERM_PRW_URW, 0, 1, 0);
    return (void*)virt;
}

void* sys_dev_rq()
{
	printf("Doing dev_rq!\r\n");
	char* dev_name = (char*)__plat_thread_getparam(curr_thread, 1);
	dev_t* dev = devmap_get_dev(dev_name);
	if(dev)
	{
		uint32_t virt_s = curr_thread->proc->brk;
		printf("Trying to map %x to %x\r\n", virt_s, dev->addr);
		pg_map(curr_thread->proc->pg_tbl, (void*)virt_s, (void*)dev->addr, 4096, 0, curr_thread->proc->priv?PERM_PRW_UNA:PERM_PRW_URW, 0, 1, 1);
		printf("Phys: %x", pg_get_phys(curr_thread->proc->pg_tbl, (void*) virt_s));
		void* old_brk = (void*)curr_thread->proc->brk;
		curr_thread->proc->brk = virt_s + 4096;
		return old_brk;
	}
	else
		return (void*)-1;
}
