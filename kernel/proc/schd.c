#include "kernel/proc/schd.h"
#include "kernel/proc/thread.h"
#include "kernel/proc/syscall.h"
#include "kernel/proc/threadqueue.h"
#include "kernel/mem/paging.h"
#include "std/string.h"
#include "std/stdlib.h"
#include "std/stdio.h"
#include <stdint.h>

thread_queue_t queues[SCHD_NUM_PRIORITIES];
thread_queue_t open_node_queue;

void schd_init()
{
	memset((char*)queues, 0, SCHD_NUM_PRIORITIES * sizeof(thread_queue_t));
	memset((char*)&open_node_queue, 0, sizeof(thread_queue_t));
	syscall_set(15, (void*)schd_chg_thread);
}

uint32_t schd_get_slice(thread_t* thread)
{
	return (SCHD_NUM_PRIORITIES-thread->priority+1)*SCHD_BASE_TIME;
}

void schd_chg_thread()
{
	if(curr_thread->state == THREAD_RUNNING)
		curr_thread->state = THREAD_READY;
	schd_add_thread(curr_thread);
	
	for(uint32_t i=0; i<SCHD_NUM_PRIORITIES; i++)
	{
		//printf("queue virt addr:%x\tqueue phys addr:%x\r\n", queues+i, pg_get_phys(&kernel_page, queues + i));
		thread_node_t* test_thread = dequeue_node(queues + i);
		
		if(test_thread)
		{
			thread_change(test_thread->data);
			test_thread->data = 0;
			test_thread->next = 0;
			enqueue_node(&open_node_queue, test_thread, 0);
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
	thread_node_t* empty_node = dequeue_node(&open_node_queue);
	if (!empty_node)
	{
		empty_node = malloc(sizeof(thread_node_t));
		uint32_t end = (((uint32_t)empty_node)/min_alloc + 1) * min_alloc; //tells you the end of what's actually been allocated
		memset((char*)empty_node, 0, end - (uint32_t)empty_node);
		
		//fit as many nodes into queue as possible from allocation
		for(uint32_t i=1; empty_node + i < (thread_node_t*)end; i++)
		{
			enqueue_node(&open_node_queue, empty_node + i, 0);
		}
	}
	//printf("Dequeued node: %x\r\n", empty_node);
	
	return empty_node;
}