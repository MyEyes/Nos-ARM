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
	//__asm__("udf 0");
	//Change to user mode
	
	printf("stdout addr: %x\r\n", &stdout);
	printf("Page info: %x\r\n", mem_get_entry(KERNEL_DEF_PG_LOC,(uint32_t *)UART0_BASE_ADDR));
	printf("Domain Flags: %x\r\n", domain_get_flags());
	printf("Processor state: %x\r\n", cpu_get_state());
	
	cpu_set_user();
	
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
	uint32_t instr = 0;
	__asm__("mov %0, r14":"=r"(instr));
	mem_dsb();
	mem_dmb();
	uart_init();
	
	uart_puts("cpu und instr:");
	uart_puthex(instr);
	uart_puts("\r\n");
	
	uart_puts("stdout addr:");
	uart_puthex((uint32_t)&stdout);
	uart_puts("\r\n");
	uart_puts("stdout putc:");
	uart_puthex((uint32_t)stdout.ops.putc);
	uart_puts("\r\n");
	uart_puts("Processor state:");
	uart_puthex(cpu_get_state());
	uart_puts("\r\n");
	uart_puts("Saved processor state:");
	uart_puthex(cpu_get_saved_state());
	uart_puts("\r\n");
	uart_puts("An interrupt happened and I don't know what to do.\r\nAAAAAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHH\r\n");
	reset();
}