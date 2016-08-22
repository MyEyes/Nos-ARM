#include "kernel/proc/thread.h"
#include "kernel/cpu/reg.h"
#include "kernel/proc/schd.h"
#include "kernel/mem/mmu.h"
#include "kernel/mem/mem.h"
#include "std/stdio.h"
#include "kernel/mem/cache.h"

thread_t kern_thread;

thread_t* curr_thread;

void thread_curr_store(char* pc, char* sp)
{
	thread_store(curr_thread, pc, sp);
}

void thread_store(thread_t* thread, char* pc, char* sp)
{
	if(!thread)
		return;
	thread->pc = pc;
	thread->sp = sp;
}

char* thread_curr_sp()
{
	return curr_thread->sp;
}

void thread_init(thread_t* thread, proc_hdr_t* proc, char* stack_start, char* stack_end, char* entry, uint32_t id)
{
	thread->tid=id;
	thread->proc=proc;
	thread->priority = 0;
	thread->state=THREAD_INIT;
	thread->stack_start = stack_start;
	thread->stack_end = stack_end;
	thread->sp=stack_start;
	thread->pc=entry;
}

void thread_ready(thread_t* thread)
{
	__plat_thread_ready(thread);
	thread->state=THREAD_READY;
}

void thread_change(thread_t* thread)
{
	printf("Switching to thread %x\r\n", thread->tid);
	printf("sp=%x\tpc=%x\r\n", thread->sp, thread->pc);
	if(curr_thread->proc->pg_tbl!=thread->proc->pg_tbl)
	{
		printf("Setting pagetable to %x\r\n", thread->proc->pg_tbl->addr);
		mmu_set_user_pgtbl(thread->proc->pg_tbl->addr);
	}
	curr_thread->state = THREAD_READY;
	schd_add_thread(curr_thread);
	curr_thread = thread;
	curr_thread->state = THREAD_RUNNING;
}

void thread_start(thread_t* thread)
{
	(void) thread;
}