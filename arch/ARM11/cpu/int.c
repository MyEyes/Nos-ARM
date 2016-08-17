#include "arch/ARM11/cpu/int.h"
#include "kernel/kernel.h"
#include <stdint.h>

void __attribute__((naked)) int_h()
{
	__asm__("ldr pc, [pc, %0]" :: "I"(LDR_ADDR_OFF):);
}
//This is exactly the instruction above
uint32_t int_instr = 0xe59ff018;

void set_int_hnd(char interrupt, void* hnd_addr)
{
	void **hnd_loc = (void**)(interrupt + INT_ADDR_LOC);
	*hnd_loc = hnd_addr;
}

void int_init()
{
	*((uint32_t*)INT_UND) = int_instr;
	*((uint32_t*)INT_SWI) = int_instr;
	*((uint32_t*)INT_PAB) = int_instr;
	*((uint32_t*)INT_DAB) = int_instr;
	*((uint32_t*)INT_IRQ) = int_instr;
	*((uint32_t*)INT_FIQ) = int_instr;
	
	
	set_int_hnd(INT_UND, kernel_panic);
	set_int_hnd(INT_SWI, kernel_panic);
	set_int_hnd(INT_PAB, kernel_panic);
	set_int_hnd(INT_DAB, kernel_panic);
	set_int_hnd(INT_IRQ, kernel_panic);
	set_int_hnd(INT_FIQ, kernel_panic);
}