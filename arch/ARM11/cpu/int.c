#include "arch/ARM11/cpu/int.h"
#include "kernel/kernel.h"
#include "arch/ARM11/cpu/cpu.h"
#include <stdint.h>

uint32_t fiq_stack[CPU_INT_STACK_SIZE];
uint32_t irq_stack[CPU_INT_STACK_SIZE];
uint32_t abt_stack[CPU_INT_STACK_SIZE];
uint32_t und_stack[CPU_INT_STACK_SIZE];

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

	
	__asm__ __volatile__(	"CPS %0\r\n"\
							"MOV sp, %1"
							::"i"(CPU_MODE_FIQ), "r"(fiq_stack));
	__asm__ __volatile__(	"CPS %0\r\n"\
							"MOV sp, %1"
							::"i"(CPU_MODE_IRQ), "r"(irq_stack));
	__asm__ __volatile__(	"CPS %0\r\n"\
							"MOV sp, %1"
							::"i"(CPU_MODE_ABT), "r"(abt_stack));
	__asm__ __volatile__(	"CPS %0\r\n"\
							"MOV sp, %1"
							::"i"(CPU_MODE_UND), "r"(und_stack));
							
	__asm__ __volatile__(	"CPS %0\r\n"::"i"(CPU_MODE_SVC));
	
	set_int_hnd(INT_UND, kernel_panic);
	set_int_hnd(INT_SWI, kernel_panic);
	set_int_hnd(INT_PAB, kernel_panic);
	set_int_hnd(INT_DAB, kernel_panic);
	set_int_hnd(INT_IRQ, kernel_panic);
	set_int_hnd(INT_FIQ, kernel_panic);
}