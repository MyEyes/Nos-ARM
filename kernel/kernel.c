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
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

extern unsigned char uart_getc();
extern void uart_putc(unsigned char c);
extern void uart_mod_init(void*, uint32_t);
extern int __end;

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{	
	(void) r0;
		
	(void) atags;
	
	uart_mod_init(0, 0);
		
	kernel_init();
	
	sysmap_print((sysmap_t*)&__end);
	
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
	mem_dsb();
	printf("An interrupt happened and I don't know what to do.\r\nAAAAAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHH\r\n");
	mem_dsb();
	mem_dmb();
	reset();
}