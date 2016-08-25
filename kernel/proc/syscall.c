#include "kernel/proc/syscall.h"
#include "kernel/proc/thread.h"

#include <string.h>

void *syscall_tbl[SYSCALL_TBL_MAX];

void syscall_init()
{
	memset((char*)syscall_tbl, 0, sizeof(void*)*SYSCALL_TBL_MAX);
	
	syscall_set(SYSCALL_PROC_SBRK, sbrk);
}

void syscall_set(syscall_id_t id, void* trg)
{
	if(id<SYSCALL_TBL_MAX)
		syscall_tbl[id] = trg;
}