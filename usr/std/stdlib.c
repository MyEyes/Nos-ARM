#include <stdlib.h>
#include <kernel/proc/syscall.h>
#include __PLATFORM__

void __attribute__((noreturn)) abort()
{
	int return_val = 0;
	PLATFORM_SYSCALL1(SYSCALL_THREAD_TERM, -1, return_val);
	while(1);
}

void __attribute__((noreturn)) exit(int status)
{
	int return_val = 0;
	PLATFORM_SYSCALL1(SYSCALL_THREAD_EXIT, status, return_val);
	while(1);
}

void system(const char* call)
{
	(void)call;
}