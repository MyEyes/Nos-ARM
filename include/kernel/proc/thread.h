#ifndef __THREAD_H__
#define __THREAD_H__
#include <stdint.h>
#include "kernel/proc/proc.h"
#include __PLATFORM__

typedef enum
{
	THREAD_READY,
	THREAD_SLEEP,
	THREAD_WAIT,
	THREAD_RUNNING,
	THREAD_INIT
}thread_state_t;

typedef struct
{
	uint32_t 		tid;
	proc_hdr_t*		proc;
	uint32_t		priority;
	uint32_t		rem_ticks;
	thread_state_t  state;
	char*			stack_start;
	char*			stack_end;
	char* 			sp;
	char* 			pc;
}thread_t;

extern thread_t kern_thread;
extern thread_t* curr_thread;

void thread_curr_store(char* pc, char* sp);
void thread_store(thread_t* thread, char* pc, char* sp);
void thread_print(thread_t* thread);
void thread_change(thread_t*);

char* thread_curr_sp();

void thread_init(thread_t* thread, proc_hdr_t* proc, char* stack_start, char* stack_end, char* entry, uint32_t id, uint32_t priority);
void thread_ready(thread_t* thread);

void __plat_thread_ready(thread_t* thread);

#endif
