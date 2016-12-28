#include "kernel/init.h"

#include <stdint.h>
#include <stdio.h>

#include "kernel/mem/mmu.h"
#include "kernel/mem/mem.h"
#include "kernel/mem/paging.h"
#include "kernel/mem/cache.h"
#include "kernel/cpu/int.h"
#include "kernel/kernel.h"
#include "kernel/proc/proc.h"
#include "kernel/res/ipc.h"
#include "kernel/proc/thread.h"
#include "kernel/proc/schd.h"
#include "kernel/proc/syscall.h"
#include "kernel/mod/kernel_uart.h"
#include "kernel/res/res_mgr.h"
#include __PLATFORM__

#define INIT_DEBUG

extern char* __start;

void kernel_init()
{	
	syscall_init();
#ifdef INIT_DEBUG	
	printf("Set up syscalls\r\n", curr_thread);
#endif

	mem_phys_init((uint32_t)PLATFORM_TOTAL_MEMORY);
	mem_phys_reset();
	#ifdef INIT_DEBUG	
	printf("Started tracking phys mem \r\n");
	#endif
	mem_phys_set(0, PLATFORM_KERNEL_PHYS_SIZE);
	mem_mark_as_kernel(0, PLATFORM_KERNEL_PHYS_SIZE);
	

#ifdef INIT_DEBUG	
	printf("Kernel memory marked as used and kernel \r\n");
#endif
	
	#ifdef INIT_DEBUG
	printf("Setting up page tables\r\n");
	#endif
	
	mem_dmb();
	pg_create_default(KERNEL_DEF_PG_LOC, KERNEL_DEF_USR_PG_LOC);
	mem_dmb();
	
	#ifdef INIT_DEBUG
	printf("Assigning page tables\r\n");
	#endif	
	
	mmu_set_kern_pgtbl(KERNEL_DEF_PG_LOC);
	
	mmu_set_user_pgtbl(KERNEL_DEF_PG_LOC);
		
	mmu_set_user_limit((uint32_t)PLATFORM_PROC_MAX_MEM);
	
	cache_TLB_flush();
	
	#ifdef INIT_DEBUG
	printf("Setting user memory limit\r\n");
	#endif
	
	//Become the domain manager, so that the MMU won't hate us
	//Sort of arch dependent wether it exists. But we might as well use it and replace it with a stub
	//if the arch does not support domains
	
	//We should already be domain manager from the kernel remap
	domain_manager_set();
#ifdef INIT_DEBUG
	printf("Became domain manager\r\n");
#endif	
	//Enable the MMU
	//Should already be enabled from kernel remap
	mmu_enable();
#ifdef INIT_DEBUG	
	printf("MMU enabled\r\n");
#endif

	#ifdef INIT_DEBUG
	printf("Setting up interrupt handlers\r\n");
	#endif

	int_init();
	
	#ifdef INIT_DEBUG
	printf("\tDone!\r\n");
	#endif	

	proc_init(&kern_proc, &user_page, 0, 0, 1);
	thread_init(&kern_thread, &kern_proc, (char*)__start, (char*)0, (char*)__start, 0, SCHD_NUM_PRIORITIES-1);
	
	curr_thread = &kern_thread;
	curr_thread->state = THREAD_RUNNING;

    kipc_init();

    res_mgr_init();
	
	schd_init();

#ifdef INIT_DEBUG	
	printf("Kernel thread initialized at %x\r\n", curr_thread);
#endif
}
