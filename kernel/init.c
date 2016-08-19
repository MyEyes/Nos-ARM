#include "kernel/init.h"

#include <stdint.h>
#include "kernel/mem/mmu.h"
#include "kernel/mem/mem.h"
#include "kernel/mem/paging.h"
#include "std/stdio.h"
#include "kernel/cpu/int.h"
#include "kernel/kernel.h"
#include "kernel/proc/proc.h"
#include "kernel/proc/thread.h"
#include "kernel/mod/kernel_uart.h"

#define INIT_DEBUG

extern char* __start;

void kernel_init()
{
	
	#ifdef INIT_DEBUG
	uart_puts("Setting up page tables\r\n");
	#endif	
	
	pg_create_default(KERNEL_DEF_PG_LOC, KERNEL_DEF_USR_PG_LOC);
	
	#ifdef INIT_DEBUG
	uart_puts("Setting user memory limit\r\n");
	#endif	

	mmu_set_user_limit((uint32_t)PLATFORM_PROC_MAX_MEM);
	
	#ifdef INIT_DEBUG
	uart_puts("Assigning page tables\r\n");
	#endif	
	
	mmu_set_user_pgtbl(KERNEL_DEF_USR_PG_LOC);
	mmu_set_kern_pgtbl(KERNEL_DEF_PG_LOC);
	
	//Become the domain manager, so that the MMU won't hate us
	//Sort of arch dependent wether it exists. But we might as well use it and replace it with a stub
	//if the arch does not support domains
	domain_manager_set();
#ifdef INIT_DEBUG
	uart_puts("Became domain manager\r\n");
#endif	
	//Enable the MMU
	mmu_enable();
#ifdef INIT_DEBUG	
	uart_puts("MMU enabled\r\n");
#endif

	mem_phys_init(PLATFORM_TOTAL_MEMORY);
#ifdef INIT_DEBUG	
	uart_puts("Started tracking phys mem \r\n");
#endif
	mem_phys_set(0, PLATFORM_KERNEL_PHYS_SIZE);
	mem_mark_as_kernel(0, PLATFORM_KERNEL_PHYS_SIZE);
#ifdef INIT_DEBUG	
	uart_puts("Kernel memory marked as used and kernel \r\n");
#endif

	proc_init(&kern_proc, &user_page, 0);
	thread_init(&kern_thread, &kern_proc, (char*)__start, (char*)0, (char*)__start, 0);
	
	curr_thread = &kern_thread;

#ifdef INIT_DEBUG	
	uart_puts("Kernel thread initialized\r\n");
#endif

	int_init();
}
