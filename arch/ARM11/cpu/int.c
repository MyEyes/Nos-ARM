#include "arch/ARM11/cpu/int.h"
#include "kernel/kernel.h"
#include "kernel/proc/thread.h"
#include "arch/ARM11/cpu/cpu.h"
#include "kernel/cpu/cpu.h"
#include "std/stdio.h"
#include "kernel/mem/paging.h"
#include "kernel/mem/perm.h"
#include "kernel/proc/schd.h"
#include "kernel/proc/syscall.h"
#include "kernel/cpu/clock.h"
#include <stdint.h>
//#define  INT_SETUP_DBG

uint32_t fiq_stack[CPU_INT_STACK_SIZE];
uint32_t irq_stack[CPU_INT_STACK_SIZE];
uint32_t abt_stack[CPU_INT_STACK_SIZE];
uint32_t und_stack[CPU_INT_STACK_SIZE];
uint32_t user_stack[SYS_INT_STACK_SIZE];

void __attribute__((naked)) int_h()
{
	__asm__("ldr pc, [pc, %0]" :: "I"(LDR_ADDR_OFF):);
}
//This is exactly the instruction above
uint32_t int_instr = 0xe59ff018;

void set_int_hnd(char interrupt, void* hnd_addr)
{
	void **hnd_loc = (void**)(interrupt + INT_PHYS_ADDR + INT_ADDR_LOC);
	*hnd_loc = hnd_addr;
}

uint32_t __attribute__((used)) irq_hnd2(char* pc, char* sp)
{
	thread_curr_store(pc, sp);
	printf("IRQ pc=%x sp=%x\r\n",pc,sp);
	//printf("Clock tick!\r\n");
	clock_clear();
	schd_chg_thread();
	return (uint32_t)thread_curr_sp();
}

void __attribute__((naked)) irq_hnd()
{
	__asm__(
	"sub lr, lr, #4\n"
	"CPS #31\n"							//Change to System mode to get access to user stack								irq
	"stmfd sp!, {r0-r12}\n"				//push register states onto stack												push(r0-r12) 					sys stack
	"mov r1, sp\n"						//mov sps value into r1 so it can survive the mode change						r1=sp_sys
	
	"CPS #18\n"							//Enter irq mode																irq
	
	"mrs r0, spsr\n"					//mov stored process state into r0												r0=spsr
	"stmfd r1!, {r0}\n"					//push value onto stack of process												push (r1=sp_sys) ro=spsr_irq 	sys stack
	
	"mov r0, lr\n"						//set up arguments for swi_hnd2 call, r1 already contains the stack pointer		r0=lr_SRV
	"stmfd r1!, {r0}\n"					//push value onto stack of process												push (r1=sp_sys) r0=lr_irq		sys stack
	
	"bl irq_hnd2\n"						//call swi_hnd2																	r0=lr_irq, r1=sp_sys
	
	"CPS #31\n"							//become system again															irq
	"mov sp, r0\n"						//Set up sp to return value
	"add sp, sp, #60\n"					//Correct for popping off stack with r12 reg 60=13*4+4+4
	"mov r12, r0\n"						//mov return value in as fake stack pointer of process							sp=swi_hnd2
	
	"mov r0, #0\n"
	"MCR p15, 0, r0, c8, c7, 0\n"		//flush TLB
	
	"CPS #18\n"							//become irq again
	"ldmfd r12!, {lr}\n"				//pop stored lr off																lr_sys=lr_irq					sys stack
	
	"ldmfd r12!, {r0}\n"				//pop stored process state off													r0=spsr_irq						sys stack
	"msr spsr, r0\n"					//Set process state																spsr=r0
	"ldmfd r12, {r0-r12}\n"				//load registers																pop(r0-r12)						sys stack
	"movs pc, lr"						//return from interrupt															pc=lr
	);
}


void __attribute__((naked)) panic_hnd()
{
	__asm__	(
	"mov r0, sp\n"
	"mov r1, lr\n"
	"bl kernel_panic\n"
	);
}

void __attribute__((used)) dabt_hnd2(uint32_t lr, uint32_t addr, uint32_t fault)
{
	printf("Data abort! @%x->%x\r\nFault: %x\r\n",lr, addr, fault);
}

void __attribute__((naked)) dabt_hnd()
{
	__asm__	(
	"sub lr, #4\n"
	"stmfd sp!, {r0-r12, lr}\n"
	"mov r0, lr\n"
	"mrc p15, 0, r1, c6, c0, 0\n"
	"mrc p15, 0, r2, c5, c0, 0\n"
	"bl dabt_hnd2\n"
	"ldmfd sp!, {r0-r12, pc}^\n"
	);
}

//returns sp of process to return from

char* __attribute__((used)) swi_hnd2(char* pc, char* sp, uint32_t swi_num)
{
	thread_curr_store(pc, sp);
	printf("SWI pc=%x sp=%x swi=%x\r\n",pc,sp, swi_num);
	if(swi_num<SYSCALL_TBL_MAX)
	{
		uint32_t (*syscall)() = syscall_tbl[swi_num];
		if(syscall)
			__plat_thread_setparam(curr_thread, 1, syscall());
		else
			printf("No associated syscall with swi %x\r\n", swi_num);
	}
	else
		printf("No associated syscall with swi %x\r\n", swi_num);
	
	char* test = thread_curr_sp();
	//printf("thread_stored_sp = %x\r\n", test);
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
	"ldr r2, [lr,#-4]\n"
	"bic r2, r2, #0xff000000\n"			//Get swi parameter
	
	"bl swi_hnd2\n"						//call swi_hnd2																	r0=lr_SRV, r1=sp_sys
	
	"CPS #31\n"							//become system again															SYS
	"mov sp, r0\n"						//Set up sp to return value
	"add sp, sp, #60\n"					//Correct for popping off stack with r12 reg 60=13*4+4+4
	"mov r12, r0\n"						//mov return value in as fake stack pointer of process							sp=swi_hnd2
	
	"mov r0, #0\n"
	"MCR p15, 0, r0, c8, c7, 0\n"		//flush TLB
	
	"CPS #19\n"							//become supervisor again
	"ldmfd r12!, {lr}\n"				//pop stored lr off																lr_sys=lr_SRV					sys stack
	
	"ldmfd r12!, {r0}\n"				//pop stored process state off													r0=spsr_SRV						sys stack
	"msr spsr, r0\n"					//Set process state																spsr=r0
	"ldmfd r12, {r0-r12}\n"				//load registers																pop(r0-r12)						sys stack
	"movs pc, lr"						//return from interrupt															pc=lr
	);
}

void int_init()
{
	*((uint32_t*)(INT_PHYS_ADDR+INT_UND)) = int_instr;
	*((uint32_t*)(INT_PHYS_ADDR+INT_SWI)) = int_instr;
	*((uint32_t*)(INT_PHYS_ADDR+INT_PAB)) = int_instr;
	*((uint32_t*)(INT_PHYS_ADDR+INT_DAB)) = int_instr;
	*((uint32_t*)(INT_PHYS_ADDR+INT_IRQ)) = int_instr;
	*((uint32_t*)(INT_PHYS_ADDR+INT_FIQ)) = int_instr;
	
	

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
							"i"(CPU_MODE_SYS), "r"(user_stack+SYS_INT_STACK_SIZE):"r8");
	#ifdef INT_SETUP_DBG
	printf("Setting interrupt stacks\r\n");
	printf("\tsp_fiq=%x\r\n", fiq_stack+CPU_INT_STACK_SIZE);
	printf("\tsp_irq=%x\r\n", irq_stack+CPU_INT_STACK_SIZE);
	printf("\tsp_abt=%x\r\n", abt_stack+CPU_INT_STACK_SIZE);
	printf("\tsp_und=%x\r\n", und_stack+CPU_INT_STACK_SIZE);
	printf("\tsp_sys=%x\r\n", user_stack+SYS_INT_STACK_SIZE);
	#endif
	
	set_int_hnd(INT_UND, (void*)panic_hnd);
	set_int_hnd(INT_SWI, (void*)swi_hnd);
	set_int_hnd(INT_PAB, (void*)panic_hnd);
	set_int_hnd(INT_DAB, (void*)dabt_hnd);
	set_int_hnd(INT_IRQ, (void*)irq_hnd);
	set_int_hnd(INT_FIQ, (void*)panic_hnd);
	
	pg_map(&kernel_page,(char*)INT_HIGH_VEC_ADDR, (char*)INT_PHYS_ADDR, 0x1000, 0, PERM_PRW_UR, 0, 1, 0);
	#ifdef INT_SETUP_DBG
	uint32_t test_addr = (uint32_t)pg_get_phys(&kernel_page, (char*)INT_HIGH_VEC_ADDR);
	printf("test = %x\r\n", test_addr);
	#endif
}