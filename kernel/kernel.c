#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include __PLATFORM__

#include "kernel/init.h"
#include "kernel/mem/mem.h"
#include "kernel/mem/paging.h"
#include "kernel/hw/reset.h"
#include "kernel/hw/devmap.h"
#include "kernel/proc/elf.h"
#include "kernel/proc/sysmap.h"
#include "kernel/mod/kernel_uart.h"
#include "kernel/cpu/cpu.h"
#include "kernel/cpu/clock.h"
#include "kernel/mem/mmu.h"
#include "kernel/proc/proc.h"
#include "kernel/proc/thread.h"
#include "kernel/proc/schd.h"
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

extern unsigned char uart_getc();
extern void uart_putc(unsigned char c);
extern void uart_mod_init(void*, uint32_t);
extern unsigned int __start;
extern unsigned int __end;
extern FILE stdout;

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{	
	(void) r0;
	
	uart_mod_init(0, 0);
	
	printf("Starting init\r\n");
	
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
	
	printf("ATAGS: %x \r\n\r\n", atags);	
	
	//printf("Look mommy, I'm running on real hardware!\r\n");
	
	//mem_dsb();
	//mem_dmb();
	//Change to user mode
	
	//printf("Domain Flags: %x\r\n", domain_get_flags());
	mem_dsb();
	mem_dmb();
	printf("Processor state: %x\r\n", cpu_get_state());
	printf("Control registers: %x\r\n", cpu_get_ctrl_regs());
	
	//void* dummy = (void*)0xff000000;
	//pg_unmap(&kernel_page, dummy, 4);
    
    domain_user_set();

	printf("Processor state: %x\r\n", cpu_get_state());
	printf("Control registers: %x\r\n", cpu_get_ctrl_regs());
	
	void* kern_end = &__end;
	
	#ifdef __DEV_MAP
		devmap = (devmap_t*)kern_end;
		printf("Devmap at %x\r\n", devmap);
		for(uint32_t x=0; x<devmap->num_devs; x++)
			printf("\t%x %s\t%s\r\n", devmap->devs[x].addr, devmap->devs[x].type, devmap->devs[x].name);
		kern_end += devmap->total_size;
	#endif
	
	printf("Memory page: %x\r\n", pg_get_entry(&kernel_page, (void*)0xc0000000));
	
	pg_tbl_t* test_tbl = proc_create((char*)0x100000, (char*)0x200000, 0x8000);
	//printf("tbl_entry: %x\r\n", pg_get_entry(test_tbl, (void*)0x000FFFC4));
	char* phys = pg_get_phys(test_tbl, (char*)0x100000);
	
	//printf("Test proc table set up at %x\r\n", test_tbl);
	//printf("Test proc phys entry at %x\r\n", phys);
	//printf("Copying image from %x to %x\r\n", &__end, phys);
	//printf("__end:%x\r\n", (uint32_t)__end);
	
	memcpy(phys, (char*)kern_end, 0x100000);
	
	proc_hdr_t* test_proc = malloc(sizeof(proc_hdr_t));
	proc_init(test_proc, test_tbl, 0x100000, 0x200000, 0);
	
	thread_t* test_thread = malloc(sizeof(thread_t));
	
	thread_init(test_thread, test_proc, (char*)0x100000, (char*)0x100000-0x8000, (char*)0x100000, 1, 0);
	
	thread_ready(test_thread);
	
	schd_add_thread(test_thread);
	
	printf("Running\r\n");
	
	//clock_enable();
	
	__asm__("swi 16"); //switch process syscall
	
	printf("Returned to kernel\r\n");
	printf("Rebooting\r\n");
	
	reset();
}

void kernel_panic(uint32_t sp, uint32_t pc)
{
	mem_dsb();
	mem_dmb();
	mmu_set_user_pgtbl(kernel_page.addr);
	uart_mod_init(0,0);
	printf("An interrupt happened and I don't know what to do.\r\nAAAAAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHH\r\n");
	printf("sp: %x\tpc: %x\r\n", sp, pc);
	printf("Processor state: %x\r\n", cpu_get_state());
	printf("Saved processor state: %x\r\n", cpu_get_saved_state());
	
	for(uint32_t x=10; x>0; x--)
	{
		printf("Restart in: %x\r", x);
		delay(0x1000000);
	}
	reset();
}
