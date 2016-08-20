#include "kernel/proc/thread.h"
#include "kernel/cpu/reg.h"
#include "kernel/proc/schd.h"

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
	thread->stack_start = stack_start;
	thread->stack_end = stack_end;
	thread->sp=stack_start;
	thread->pc=entry;
}

void thread_change(thread_t* thread)
{
	curr_thread = thread;
}

void thread_start(thread_t* thread)
{
	(void) thread;
}