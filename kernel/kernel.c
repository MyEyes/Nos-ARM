#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>
#include __PLATFORM__
#include "kernel/init.h"
#include "kernel/mem/mem.h"
#include "kernel/mem/paging.h"
#include "std/stdio.h"
#include "std/stdlib.h"
#include "kernel/hw/reset.h"
#include "kernel/proc/elf.h"
#include "kernel/proc/sysmap.h"
#include "kernel/mod/kernel_uart.h"
#include "kernel/cpu/cpu.h"
#include "kernel/mem/mmu.h"
#include "kernel/proc/proc.h"
#include "kernel/proc/thread.h"
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

extern unsigned char uart_getc();
extern void uart_putc(unsigned char c);
extern void uart_mod_init(void*, uint32_t);
extern int __start;
extern int __end;
extern FILE stdout;

void test()
{
	printf("Proc, started");
}

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{	
	(void) r0;
	
	uart_mod_init(0, 0);
	
	printf("%x\r\n", cpu_get_state());
			
	kernel_init();

	printf("Hello, kernel World!\r\n\r\n");
	
	if(r1==PLATFORM_CPU_MACHINE_TYPE)
		printf("Found correct CPU type\r\n");
	else
	{
		printf("Unexpected CPU type detected\r\n");
		printf("Expected:");
		printf("%x", PLATFORM_CPU_MACHINE_TYPE);
		printf("\r\nDetected:");
		printf("%x", r1);
		printf("\r\n\r\n");
	}
	
	printf("ATAGS: ");
	printf("%x", atags);
	printf("\r\n\r\n");
	
	printf("Look mommy, I'm running on real hardware!\r\n");
	
	mem_dsb();
	mem_dmb();
	//Change to user mode
	
	printf("Domain Flags: %x\r\n", domain_get_flags());
	mem_dsb();
	mem_dmb();
	printf("Processor state: %x\r\n", cpu_get_state());
	//printf("Control registers: %x\r\n", cpu_get_ctrl_regs());
	
	//cpu_set_user();
	
	//__asm__("udf 0");
	//void* dummy = (void*)0xff000000;
	//pg_unmap(&kernel_page, dummy, 4);
	
	//*((uint32_t*)dummy) = 0;
	
	//__asm__("swi 0");
	
	//printf("Processor state (User): %x\r\n", cpu_get_state());
	
	printf("Hello user mode!\r\n");

	/*
	proc_hdr_t* test_proc = malloc(sizeof(proc_hdr_t));
	proc_init(test_proc, &kernel_page, 1);
	thread_t* test_thread = malloc(sizeof(thread_t));
	char* stack = malloc(0x8000);
	thread_init(test_thread, test_proc, stack+0x8000, stack, (char*)test, 1);
	*/
 
	
	while ( true )
	{
		char c = uart_getc();
		if(c=='r')
			reset();
		else
			uart_putc(c);
	}
	
	reset();
}

void kernel_panic()
{
	//mem_dsb();
	//mem_dmb();
	uart_init();
	printf("Processor state: %x\r\n", cpu_get_state());
	printf("Saved processor state: %x\r\n", cpu_get_saved_state());
	printf("An interrupt happened and I don't know what to do.\r\nAAAAAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHH\r\n");
	reset();
}
