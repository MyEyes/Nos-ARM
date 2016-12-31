#include <stdio.h>
#include <stdlib.h>
#include <kernel/proc/syscall.h>
#include "usr/std/memory.h"
#include __PLATFORM__

void* req_res(char* res_name, void* addr)
{
	void* res = 0;
	//request device
	PLATFORM_SYSCALL2(SYSCALL_RES_RQ_NM, res_name, addr, res);
	return res;
}
