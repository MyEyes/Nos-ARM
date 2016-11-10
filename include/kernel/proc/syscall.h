#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <stdint.h>
#include <sys/types.h>

#define SYSCALL_THREAD_YIELD 16
#define SYSCALL_THREAD_EXIT 17
#define SYSCALL_THREAD_TERM 18

#define SYSCALL_PROC_SBRK 24
#define SYSCALL_PROC_FORK 25

#define SYSCALL_PMAP 64

#define SYSCALL_DEV_RQ 128

#define SYSCALL_TBL_MAX 512
extern void *syscall_tbl[];

typedef uint32_t syscall_id;

void syscall_init();
void syscall_set(syscall_id id, void* target);

void* sys_dev_rq();

#endif