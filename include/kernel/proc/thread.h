#ifndef __THREAD_H__
#define __THREAD_H__
#include <stdint.h>
#include "kernel/proc/proc.h"
#include __PLATFORM__

typedef struct
{
	uint32_t 	tid;
	proc_hdr_t*	proc;
	char*		stack_start;
	char*		stack_end;
	char* 		sp;
	char* 		pc;
}thread_t;

extern thread_t kern_thread;
extern thread_t* curr_thread;

void thread_curr_store(char* pc, char* sp);
void thread_store(thread_t* thread, char* pc, char* sp);
void thread_print(thread_t* thread);

char* thread_curr_sp();

void thread_init(thread_t* thread, proc_hdr_t* proc, char* stack_start, char* stack_end, char* entry, uint32_t id);

#endif