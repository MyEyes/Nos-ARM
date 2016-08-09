#include "kernel/init.h"

#include <stdint.h>
#include "kernel/mem/mmu.h"
#include "kernel/mem/mem.h"
#include "kernel/mem/fld.h"
#include "kernel/util/kernel_uart.h"
#include "kernel/cpu/int.h"
#include "kernel/kernel.h"

void kernel_init()
{
	//Set up identity mapped page tables
	create_flat_fld0(KERNEL_FLD0_LOC, 0, PLATFORM_PROC_MAX_MEM);
	if(PLATFORM_TTBCR_N!=0)
	{
		create_flat_fld1(KERNEL_FLD1_LOC, 0, PLATFORM_TOTAL_ADDR_RANGE);
#ifdef INIT_DEBUG
		uart_puts("Flat Page Tables set up\r\n");
#endif
	}
	//Set correct n value for TTBRC
	mmu_set_ttbrc(PLATFORM_TTBCR_N);
#ifdef INIT_DEBUG
	uart_puts("TTBRC set\r\n");
#endif
	//Set up TTBR0 and 1 if necessary
	mmu_set_ttbr0(KERNEL_FLD0_LOC);
#ifdef INIT_DEBUG
	uart_puts("TTBR0 set\r\n");
#endif
	if(PLATFORM_TTBCR_N!=0)
	{
		mmu_set_ttbr1(KERNEL_FLD1_LOC);
#ifdef INIT_DEBUG
		uart_puts("TTBR1 set\r\n");
#endif
	}
	
	//Become the domain manager, so that the MMU won't hate us
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

	int_init();
	set_int_hnd(INT_UND, kernel_panic);
	set_int_hnd(INT_SWI, kernel_panic);
	set_int_hnd(INT_PAB, kernel_panic);
	set_int_hnd(INT_DAB, kernel_panic);
	set_int_hnd(INT_IRQ, kernel_panic);
	set_int_hnd(INT_FIQ, kernel_panic);
}