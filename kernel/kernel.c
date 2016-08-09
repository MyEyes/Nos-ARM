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
	
	uint32_t* fld_tbl_loc = (uint32_t*)KERNEL_FLD1_LOC;
	void* test_addr = (void*)(PLATFORM_KERNEL_BASE-0x100000);
	uart_puthex((uint32_t)fld_tbl_loc);
	uart_puts("\r\n");
	uart_puthex((uint32_t)test_addr);
	uart_puts("\r\n");
	uart_puthex((uint32_t)mem_get_phys(fld_tbl_loc, test_addr));
	uart_puts("\r\n");
	
	mem_unmap(fld_tbl_loc, test_addr, 1<<12);
	uart_puthex((uint32_t)mem_get_phys(fld_tbl_loc, test_addr));
	uart_puts("\r\n");
	
	uart_puthex((uint32_t)mem_get_phys(fld_tbl_loc, test_addr+(1<<12)));
	uart_puts("\r\n");
	
	mem_map(fld_tbl_loc, test_addr, (void*) 0xFEF00000, 1<<12, 0, 0, 0, 1, 0);
	uart_puthex((uint32_t)mem_get_phys(fld_tbl_loc, test_addr));
	uart_puts("\r\n");
	
	uart_puthex((uint32_t)mem_get_phys(fld_tbl_loc, test_addr+(1<<12)));
	uart_puts("\r\n");
	
	uart_puthex(*((uint32_t*)0x4));
	uart_puts("\r\n");
	uart_puthex(*((uint32_t*)0x24));
	uart_puts("\r\n");
	
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