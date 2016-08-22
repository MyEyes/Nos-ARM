#ifndef __THREADQUEUE_H__
#define __THREADQUEUE_H__
#include "kernel/proc/thread.h"

typedef struct thread_node_t
{
	thread_t* data;
	struct thread_node_t* next;
}thread_node_t;

typedef struct
{
	thread_node_t* head;
	thread_node_t* tail;
}thread_queue_t;

void enqueue_node(thread_queue_t* queue, thread_node_t* node, thread_t* thread);
thread_node_t* dequeue_node(thread_queue_t* queue);

#endif