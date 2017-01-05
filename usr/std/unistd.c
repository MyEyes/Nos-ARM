#include <unistd.h>
#include "kernel/proc/syscall.h"
#include __PLATFORM__

void* sbrk(size_t size)
{
	void* result = 0;
	PLATFORM_SYSCALL1(SYSCALL_PROC_SBRK, size, result);
	return result;
}

void* pmap(void* p_addr, void* v_addr, size_t mem)
{
    void* result = 0;
    PLATFORM_SYSCALL3(SYSCALL_PMAP, p_addr, v_addr, mem, result);
    return result;
}

uint32_t fork()
{
	uint32_t result = 0;
	PLATFORM_SYSCALL(SYSCALL_PROC_FORK, result);
	return result;
}

uint32_t thread_create(void* stack_start, void* stack_end, void* entry)
{
    uint32_t result = 0;
    PLATFORM_SYSCALL3(SYSCALL_THREAD_CREATE, stack_start, stack_end, entry, result);
    return result;
}

uint32_t thread_run(uint32_t tid)
{
    uint32_t result = 0;
    PLATFORM_SYSCALL1(SYSCALL_THREAD_RUN, tid, result);
    return result;
}
