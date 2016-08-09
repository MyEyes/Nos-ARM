#include "kernel/cpu/int.h"
#include <stdint.h>

void __attribute__((naked)) int_h()
{
	__asm__("ldr pc, [pc, %0]" :: "I"(LDR_ADDR_OFF):);
}
//This is exactly the instruction above
uint32_t instr = 0xe59ff018;

void set_int_hnd(char interrupt, void* hnd_addr)
{
	void **hnd_loc = (void**)(interrupt + INT_ADDR_LOC);
	*hnd_loc = hnd_addr;
}

void int_init()
{
	*((uint32_t*)INT_UND) = instr;
	*((uint32_t*)INT_SWI) = instr;
	*((uint32_t*)INT_PAB) = instr;
	*((uint32_t*)INT_DAB) = instr;
	*((uint32_t*)INT_IRQ) = instr;
	*((uint32_t*)INT_FIQ) = instr;
}