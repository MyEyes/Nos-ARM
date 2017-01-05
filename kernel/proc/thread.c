#include <stdio.h>
#include <stdlib.h>
#include "kernel/proc/thread.h"
#include "kernel/cpu/reg.h"
#include "kernel/cpu/clock.h"
#include "kernel/proc/schd.h"
#include "kernel/mem/mmu.h"
#include "kernel/mem/mem.h"
#include "kernel/mem/cache.h"

//#define DBG_THREADS

thread_t kern_thread;

thread_t* curr_thread;

int tid_counter = 1000;

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

void thread_init(thread_t* thread, proc_hdr_t* proc, char* stack_start, char* stack_end, char* entry, uint32_t id, uint32_t priority)
{
	thread->tid=id;
	thread->proc=proc;
	thread->priority = priority;
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

int sys_create_thread()
{
    thread_t* caller_thread = curr_thread;
    thread_t* new_thread = malloc(sizeof(thread_t));
    int new_tid = tid_counter++;

    void* stack_start = (void*) __plat_thread_getparam(curr_thread, 1);
    void* stack_end = (void*) __plat_thread_getparam(curr_thread, 2);
    void* entry = (void*) __plat_thread_getparam(curr_thread, 3);

    thread_init(new_thread, caller_thread->proc, stack_start, stack_end, entry, new_tid, caller_thread->priority);

    schd_add_thread(new_thread);

    return new_tid;
}


void thread_change(thread_t* thread)
{
	#ifdef DBG_THREADS
	printf("Switching to thread %x\r\n", thread->tid);
	printf("sp=%x\tpc=%x\r\n", thread->sp, thread->pc);
	#endif
	
	if(curr_thread->proc->pg_tbl!=thread->proc->pg_tbl)
	{
		#ifdef DBG_THREADS
		printf("Setting pagetable to %x\r\n", pg_get_phys(&kernel_page, thread->proc->pg_tbl->addr));
		#endif
		mmu_set_user_pgtbl(pg_get_phys(&kernel_page, thread->proc->pg_tbl->addr));
	}
	
	curr_thread = thread;
	curr_thread->state = THREAD_RUNNING;
	clock_set(schd_get_slice(curr_thread));
}

void thread_start(thread_t* thread)
{
	(void) thread;
}
