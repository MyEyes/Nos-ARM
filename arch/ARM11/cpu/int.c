#include "arch/ARM11/cpu/int.h"
#include "kernel/kernel.h"
#include "kernel/proc/thread.h"
#include "arch/ARM11/cpu/cpu.h"
#include "std/stdio.h"
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

void __attribute__((used)) test_hnd2()
{
	printf("Data abort!\r\n");	
}

void __attribute__((naked)) test_hnd()
{
	__asm__	(
	"stmfd sp!, {r0-r12, lr}\n"\
	"bl test_hnd2\n"\
	"ldmfd sp!, {r0-r12, lr}\n"
	);
}


//returns sp of process to return from

char* __attribute__((used)) swi_hnd2(char* pc, char* sp, uint32_t swi_num)
{
	(void)swi_num;
	thread_curr_store(pc, sp);
	printf("SWI pc=%x sp=%x\r\n",pc,sp);
	char* test = thread_curr_sp();
	printf("thread_stored_sp = %x\r\n", test);
	return test;
}

void __attribute__((naked)) swi_hnd()
{
	__asm__	(
	"stmfd sp!, {r0-r12, lr}\n"\
	"mrs r0, spsr\n"\
	"push {r0}\n"\
	"mov r1, sp\n"\
	"mov r0, lr\n"\
	"bl swi_hnd2\n"\
	"mov sp, r0\n"\
	"pop {r0}\n"\
	"msr spsr, r0\n"\
	"ldmfd sp!, {r0-r12, pc}^\n"
	);
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
	
	set_int_hnd(INT_UND, (void*)kernel_panic);
	set_int_hnd(INT_SWI, (void*)swi_hnd);
	set_int_hnd(INT_PAB, (void*)kernel_panic);
	set_int_hnd(INT_DAB, (void*)test_hnd);
	set_int_hnd(INT_IRQ, (void*)kernel_panic);
	set_int_hnd(INT_FIQ, (void*)kernel_panic);
}