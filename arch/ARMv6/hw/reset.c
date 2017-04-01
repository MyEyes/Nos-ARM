#include "arch/ARM11/hw/reset.h"
#include "kernel/mem/mem.h"
#include <stdint.h>

void reset()
{
	mem_dmb();
	mem_dsb();
	uint32_t pw = 0x5a << 24;
	*((uint32_t*)(WATCHDOG_ADDR+WD)) = pw | 8;
	*((uint32_t*)(WATCHDOG_ADDR+RSTC)) = (*((uint32_t*)(WATCHDOG_ADDR+RSTC)) & ~0x30) | pw | 0x20;
	
	while(1){}
}