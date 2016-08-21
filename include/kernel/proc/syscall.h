#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <stdint.h>

#define SYSCALL_TBL_MAX 512
extern void *syscall_tbl[];

void syscall_init();
void syscall_set(uint32_t id, void* target);

#endif