#include "kernel/proc/syscall.h"
#include "kernel/proc/thread.h"
#include "kernel/mem/paging.h"
#include "kernel/mem/perm.h"
#include "kernel/hw/devmap.h"

#include <string.h>

void *syscall_tbl[SYSCALL_TBL_MAX];

void syscall_init()
{
	memset((char*)syscall_tbl, 0, sizeof(void*)*SYSCALL_TBL_MAX);
	
	syscall_set(SYSCALL_PROC_SBRK, sbrk);
	syscall_set(SYSCALL_DEV_RQ, sys_dev_rq);
}

void syscall_set(syscall_id_t id, void* trg)
{
	if(id<SYSCALL_TBL_MAX)
		syscall_tbl[id] = trg;
}

void* sys_pmap(void* phys_addr, size_t mem)
{
	mem = (mem+4095)&4095;
	pg_map(curr_thread->proc->pg_tbl, (void*)curr_thread->proc->brk, phys_addr, mem, 0, curr_thread->proc->priv?PERM_PRW_UNA:PERM_PRW_URW, 0, 1, 0);
	void* old_brk = (void*)curr_thread->proc->brk;
	curr_thread->proc->brk += mem;
	return old_brk;
}

void* sys_dev_rq()
{
	char* dev_name = (char*)__plat_thread_getparam(curr_thread, 1);
	dev_t* dev = devmap_get_dev(dev_name);
	if(dev)
	{
		pg_map(curr_thread->proc->pg_tbl, (void*)curr_thread->proc->brk, (void*)dev->addr, 4096, 0, curr_thread->proc->priv?PERM_PRW_UNA:PERM_PRW_URW, 0, 1, 0);
	
		void* old_brk = (void*)curr_thread->proc->brk;
		curr_thread->proc->brk += 4096;
		return old_brk;
	}
	else
		return (void*)-1;
}