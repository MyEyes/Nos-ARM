#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>
#include __PLATFORM__
#include "kernel/init.h"
#include "kernel/mem/mem.h"
#include "kernel/util/kernel_uart.h"
#include "kernel/hw/reset.h"
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{	
	(void) r0;
		
	(void) atags;
 
	uart_init();
	uart_puts("Hello, kernel World!\r\n\r\n");
	
	if(r1==PLATFORM_CPU_MACHINE_TYPE)
		uart_puts("Found correct CPU type\r\n");
	else
	{
		uart_puts("Unexpected CPU type detected\r\n");
		uart_puts("Expected:");
		uart_puthex(PLATFORM_CPU_MACHINE_TYPE);
		uart_puts("\r\nDetected:");
		uart_puthex(r1);
		uart_puts("\r\n\r\n");
	}
	
	uart_puts("ATAGS: ");
	uart_puthex(atags);
	uart_puts("\r\n\r\n");
		
	kernel_init();
		
	uart_puts("Hello, MMU World!\r\n");
	
	uart_puts("Look mommy, I'm running on real hardware!\r\n");
	
	//__asm__("udf 0");
 
	while ( true )
	{
		char c = uart_getc();
		if(c=='r')
			reset();
		else
			uart_putc(c);
	}
}

void kernel_panic()
{
	uart_puts("An interrupt happened and I don't know what to do.\r\nAAAAAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHH\r\n");
	reset();
}