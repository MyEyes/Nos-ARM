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
#include "kernel/hw/reset.h"
#include "kernel/proc/elf.h"
#include "kernel/proc/sysmap.h"
#include "kernel/mod/kernel_uart.h"
#include "kernel/cpu/cpu.h"
#include "kernel/mem/mmu.h"
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

extern unsigned char uart_getc();
extern void uart_putc(unsigned char c);
extern void uart_mod_init(void*, uint32_t);
extern int __start;
extern int __end;
extern FILE stdout;

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{	
	(void) r0;
		
	(void) atags;
	
	uart_mod_init(0, 0);
			
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
	
	printf("stdout addr: %x\r\n", &stdout);
	printf("Page info: %x\r\n", pg_get_entry(KERNEL_DEF_PG_LOC,(uint32_t *)UART0_BASE_ADDR));
	printf("Domain Flags: %x\r\n", domain_get_flags());
	printf("Processor state: %x\r\n", cpu_get_state());
	printf("Control registers: %x\r\n", cpu_get_ctrl_regs());
	
	cpu_set_user();
	
	__asm__("udf 0");
	
	//printf("Processor state (User): %x\r\n", cpu_get_state());
	
	//printf("Hello user mode!\r\n");
 
	
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