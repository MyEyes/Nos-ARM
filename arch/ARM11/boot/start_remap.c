#include <stdint.h>
#include "arch/ARM11/mem/mmu.h"
#include "arch/ARM11/cpu/coproc.h"
#include __PLATFORM__

extern void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags);
extern __attribute__((noreturn)) void kernel_panic();

void __attribute__((noreturn)) kernel_map(uint32_t r0, uint32_t r1, uint32_t atags)
{	
	//mini pagetable to map the first 4MB of memory to the kernel address
	//we locate this at the top of the RAM, so that it won't get in the way until we overwrite it
	uint32_t* basemap = (void*)(PLATFORM_TOTAL_MEMORY-0x100000);
	for(uint32_t x = 0; x<PLATFORM_KERNEL_VIRT_BASE_OFFSET; x++)
		basemap[x] = (x<<20) + 2;
	for(uint32_t x = 0; x<PLATFORM_KERNEL_VIRT_SIZE; x++)
		basemap[PLATFORM_KERNEL_VIRT_BASE_OFFSET + x] = (x<<20) + 2;
		
	//Become domain manager
	MCR(SYS_CTRL, 0, 3, 0, 0, 3);
	
	//Set up TTBR0
	MCR(SYS_CTRL, 0, MMU_TTB_CTRL, 0, MMU_TTB0, (uint32_t)(PLATFORM_TOTAL_MEMORY-0x100000));
	
	//Enable MMU
	uint32_t old_val = 0;
	MRC(SYS_CTRL,0,MMU_SYSCTL_MAIN,0, 0, old_val);
	old_val |= 1;
	MCR(SYS_CTRL,0,MMU_SYSCTL_MAIN,0, 0, old_val);
	
	kernel_main(r0, r1, atags);
	kernel_panic();
}