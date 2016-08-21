#include "kernel/proc/syscall.h"

#include "std/string.h"

void *syscall_tbl[SYSCALL_TBL_MAX];

void syscall_init()
{
	memset((char*)syscall_tbl, 0, sizeof(void*)*SYSCALL_TBL_MAX);
}

void syscall_set(uint32_t id, void* trg)
{
	syscall_tbl[id] = trg;
}