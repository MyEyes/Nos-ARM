#include <stdio.h>
#include <stdlib.h>
#include <kernel/proc/syscall.h>
#include "usr/std/memory.h"
#include __PLATFORM__

void* req_dev(char* dev_name)
{
	void* res = 0;
	//request device
	PLATFORM_SYSCALL1(SYSCALL_DEV_RQ, dev_name, res);
	//If we got a device back we map the next 4kb
	if(res && res!=(void*)-1)
	{
		mem_extend(4096, 0);
	}
	return res;
}