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
uint32_t user_stack[2048];

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


void __attribute__((naked)) panic_hnd()
{
	__asm__	(
	"mov r0, sp\n"
	"mov r1, lr\n"
	"bl kernel_panic\n"
	);
}

void __attribute__((used)) test_hnd2()
{
	printf("Data abort!\r\n");	
}

void __attribute__((naked)) test_hnd()
{
	__asm__	(
	"sub lr, #4\n"
	"stmfd sp!, {r0-r12, lr}\n"
	"bl test_hnd2\n"
	"ldmfd sp!, {r0-r12, pc}^\n"
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
	
	"CPS #31\n"							//Change to System mode to get access to user stack								SYS
	"stmfd sp!, {r0-r12}\n"				//push register states onto stack												push(r0-r12) 					sys stack
	"mov r1, sp\n"						//mov sps value into r1 so it can survive the mode change						r1=sp_sys
	
	"CPS #19\n"							//Enter supervisor mode															SRV
	
	"mrs r0, spsr\n"					//mov stored process state into r0												r0=spsr
	"stmfd r1!, {r0}\n"					//push value onto stack of process												push (r1=sp_sys) ro=spsr_SRV 	sys stack
	
	"mov r0, lr\n"						//set up arguments for swi_hnd2 call, r1 already contains the stack pointer		r0=lr_SRV
	"stmfd r1!, {r0}\n"					//push value onto stack of process												push (r1=sp_sys) r0=lr_SRV		sys stack
	
	"bl swi_hnd2\n"						//call swi_hnd2																	r0=lr_SRV, r1=sp_sys
	
	"CPS #31\n"							//become system again															SYS
	"mov sp, r0\n"						//mov return value in as stack pointer of process								sp=swi_hnd2
	"ldmfd sp!, {lr}\n"					//pop stored lr off																lr_sys=lr_SRV					sys stack
	"ldmfd sp!, {r0}\n"					//pop stored process state off													r0=spsr_SRV						sys stack
	"msr spsr, r0\n"					//Set process state																spsr=r0
	"ldmfd sp!, {r0-r12}\n"				//load registers																pop(r0-r12)						sys stack
	"movs pc, lr"						//return from interrupt															pc=lr
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

	
	__asm__ __volatile__(	"mrs r8, cpsr\n"
							"CPS %0\n"
							"MOV sp, %1\n"
							"CPS %2\n"
							"MOV sp, %3\n"
							"CPS %4\n"
							"MOV sp, %5\n"
							"CPS %6\n"
							"MOV sp, %7\n"
							"CPS %8\n"
							"MOV sp, %9\n"
							"msr cpsr, r8\n"
							::"i"(CPU_MODE_FIQ), "r"(fiq_stack+CPU_INT_STACK_SIZE),
							"i"(CPU_MODE_IRQ), "r"(irq_stack+CPU_INT_STACK_SIZE),
							"i"(CPU_MODE_ABT), "r"(abt_stack+CPU_INT_STACK_SIZE),
							"i"(CPU_MODE_UND), "r"(und_stack+CPU_INT_STACK_SIZE),
							"i"(CPU_MODE_SYS), "r"(user_stack+2000):"r8");
	
	set_int_hnd(INT_UND, (void*)panic_hnd);
	set_int_hnd(INT_SWI, (void*)swi_hnd);
	set_int_hnd(INT_PAB, (void*)panic_hnd);
	set_int_hnd(INT_DAB, (void*)test_hnd);
	set_int_hnd(INT_IRQ, (void*)panic_hnd);
	set_int_hnd(INT_FIQ, (void*)panic_hnd);
}