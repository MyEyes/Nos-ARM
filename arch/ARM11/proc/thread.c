#include "kernel/mem/paging.h"
#include "kernel/proc/thread.h"
#include "std/string.h"
#include "std/stdio.h"
#include "arch/ARM11/cpu/cpu.h"
#include "kernel/mem/mem.h"
#include <stdint.h>

void __plat_thread_ready(thread_t* thread)
{
	mem_dmb();
	mem_dsb();
	uint32_t* volatile phys_stack_addr = (pg_get_phys(thread->proc->pg_tbl, (char*)thread->stack_start-4)+4);
	
	mem_dmb();
	mem_dsb();
	//printf("thread: %x\r\n", thread);
	//printf("thread->proc: %x\r\n", thread->proc);
	//printf("virt_stack_addr = %x\tphys_stack_addr = %x\r\n",thread->stack_start, phys_stack_addr);
	if(!phys_stack_addr)
		return;
	//thread suspend pushes r0-12, then spsr, then lr
	memset((char*) (phys_stack_addr-15), 0, 15*sizeof(uint32_t));
	phys_stack_addr[-15] = (uint32_t)thread->pc; //lr
	phys_stack_addr[-14] = thread->proc->priv ? CPU_MODE_SYS : CPU_MODE_USR; //spsr, with all interrupts enabled
	phys_stack_addr[-13] = thread->tid; //r0=tid (thread id)
	thread->sp -= sizeof(uint32_t)*15;
}