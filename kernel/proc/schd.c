#include "kernel/proc/schd.h"
#include "kernel/proc/thread.h"
#include "kernel/proc/syscall.h"
#include "kernel/proc/threadqueue.h"
#include "std/string.h"
#include "std/stdlib.h"
#include <stdint.h>

thread_queue_t queues[SCHD_NUM_PRIORITIES];
thread_queue_t* open_node_queue;

void schd_init()
{
	memset((char*)queues, 0, SCHD_NUM_PRIORITIES * sizeof(thread_queue_t));
	memset((char*)open_node_queue, 0, sizeof(thread_queue_t));
	syscall_set(15, (void*)schd_chg_thread);
}

void schd_chg_thread()
{
	for(uint32_t i=0; i<SCHD_NUM_PRIORITIES; i++)
	{
		thread_node_t* test_thread = dequeue_node(queues + i);
		
		if(test_thread)
		{
			thread_change(test_thread->data);
			return;
		}
	}
}

void schd_add_thread(thread_t* thread)
{
	thread_node_t* empty_node = schd_get_empty_node();
	enqueue_node(queues + thread->priority, empty_node, thread);
}

thread_node_t* schd_get_empty_node()
{
	thread_node_t* empty_node = dequeue_node(open_node_queue);
	if (!empty_node)
	{
		empty_node = malloc(sizeof(thread_node_t));
		uint32_t end = (((uint32_t)empty_node)/min_alloc + 1) * min_alloc; //tells you the end of what's actually been allocated
		memset((char*)empty_node, 0, end - (uint32_t)empty_node);
		
		//fit as many nodes into queue as possible from allocation
		for(uint32_t i=1; empty_node + i < (thread_node_t*)end; i++)
		{
			enqueue_node(open_node_queue, empty_node + i, 0);
		}
	}
	
	return empty_node;
}