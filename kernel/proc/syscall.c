#include "kernel/proc/syscall.h"

#include <string.h>

void *syscall_tbl[SYSCALL_TBL_MAX];

void syscall_init()
{
	memset((char*)syscall_tbl, 0, sizeof(void*)*SYSCALL_TBL_MAX);
}

void syscall_set(syscall_id_t id, void* trg)
{
	syscall_tbl[id] = trg;
}