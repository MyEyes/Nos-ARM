#include "kernel/cpu/clock.h"
#include <stdint.h>
#include <stdio.h>
#include "kernel/mem/mem.h"

void clock_enable()
{
	*((uint32_t*)(PLATFORM_BUS_TO_PHYS((PLATFORM_IRQ_BASE+PLATFORM_IRQ_ENA0)))) |= 1;
	*((uint32_t*)(PLATFORM_BUS_TO_PHYS((PLATFORM_CLOCK_BASE+PLATFORM_CLOCK_CONTROL)))) = 1<<7|1<<5|1<<1;
	*((uint32_t*)(PLATFORM_BUS_TO_PHYS((PLATFORM_CLOCK_BASE+PLATFORM_CLOCK_RELOAD)))) = 0xF0000;
	*((uint32_t*)(PLATFORM_BUS_TO_PHYS((PLATFORM_CLOCK_BASE+PLATFORM_CLOCK_LOAD)))) = 0xF0000;
}

void clock_clear()
{
	*((uint32_t*)(PLATFORM_BUS_TO_PHYS((PLATFORM_CLOCK_BASE+PLATFORM_CLOCK_IRQACK)))) = 1;
}

void clock_set(uint32_t time)
{
	*((uint32_t*)(PLATFORM_BUS_TO_PHYS((PLATFORM_CLOCK_BASE+PLATFORM_CLOCK_LOAD)))) = time;
	mem_dmb();
}