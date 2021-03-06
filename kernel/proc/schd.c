#include "kernel/proc/schd.h"
#include "kernel/proc/thread.h"
#include "kernel/proc/syscall.h"
#include "kernel/proc/threadqueue.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "kernel/mem/paging.h"
#include "kernel/proc/thread.h"
//#define DBG_SCHD

thread_queue_t queues[SCHD_NUM_PRIORITIES];
thread_queue_t empty_node_queue;
thread_queue_t sleep_queue;

void schd_init()
{
	memset((char*)queues, 0, SCHD_NUM_PRIORITIES * sizeof(thread_queue_t));
	memset((char*)&empty_node_queue, 0, sizeof(thread_queue_t));
	memset((char*)&sleep_queue, 0, sizeof(thread_queue_t));
	syscall_set(SYSCALL_THREAD_YIELD, (void*)schd_chg_thread);
	syscall_set(SYSCALL_THREAD_TERM, (void*)schd_term);
	syscall_set(SYSCALL_THREAD_EXIT, (void*)schd_term);
}

uint32_t schd_get_slice(thread_t* thread)
{
	return (SCHD_NUM_PRIORITIES-thread->priority+1)*SCHD_BASE_TIME;
}

void schd_chg_thread()
{
    //printf("Scheduler running!\r\n");
	if(curr_thread->state == THREAD_RUNNING)
	{
		curr_thread->state = THREAD_READY;
		schd_add_thread(curr_thread);
	}
	
	for(uint32_t i=0; i<SCHD_NUM_PRIORITIES; i++)
	{
		#ifdef DBG_SCHD
		printf("queue virt addr:%x\tqueue phys addr:%x\r\n", queues+i, pg_get_phys(&kernel_page, queues + i));
		
		#endif
		thread_node_t* test_thread = dequeue_node(queues + i);
		
		if(test_thread)
		{
			thread_change(test_thread->data);
			test_thread->data = 0;
			test_thread->next = 0;
			enqueue_node(&empty_node_queue, test_thread, 0);
			return;
		}
	}
}

thread_t* get_waiting_thread(uint32_t tid)
{    
    thread_node_t* curr_node = sleep_queue.head;
    while(curr_node)
    {
        if(curr_node->data->tid == tid)
            return curr_node->data;
        curr_node = curr_node->next;
    }
    return (thread_t*)0;
}

void schd_term()
{
	curr_thread->state = THREAD_EXIT;
	uint32_t return_val = __plat_thread_getparam(curr_thread, 1);
	printf("Process #%x, terminated with code %x\r\n", curr_thread->tid, return_val);
	schd_chg_thread();
}

int sys_run_thread()
{
    //Get passed tid from calling thread
    uint32_t tid = __plat_thread_getparam(curr_thread, 1);
    //Get thread they want us to start
    thread_t* thread = get_waiting_thread(tid);
    //Check that thread is in correct state and start
    if(thread->state == THREAD_INIT)
    {
        thread_ready(thread);
        schd_add_thread(thread);
        return 0;
    }
    return -1;
}

void schd_add_thread(thread_t* thread)
{
	thread_node_t* empty_node = schd_get_empty_node();
	#ifdef DBG_SCHD
	printf("Adding thread %x to queue %x\r\n",thread->tid, thread->priority);
	printf("pc: %x\tsp: %x\r\n", (uint32_t)thread->pc, (uint32_t)thread->sp);
	#endif
    if(thread->state == THREAD_READY)
    	enqueue_node(queues + thread->priority, empty_node, thread);
    else if(thread->state == THREAD_SLEEP || thread->state == THREAD_INIT)
        enqueue_node(&sleep_queue, empty_node, thread);
}

thread_node_t* schd_get_empty_node()
{
	thread_node_t* empty_node = dequeue_node(&empty_node_queue);
	if (!empty_node)
	{
		empty_node = malloc(sizeof(thread_node_t));
		uint32_t end = (((uint32_t)empty_node)/min_alloc + 1) * min_alloc; //tells you the end of what's actually been allocated
		memset((char*)empty_node, 0, end - (uint32_t)empty_node);
		
		//fit as many nodes into queue as possible from allocation
		for(uint32_t i=1; empty_node + i + 1 < (thread_node_t*)end; i++)
		{
			enqueue_node(&empty_node_queue, empty_node + i, 0);
		}
		#ifdef DBG_SCHD
		printf("tail: %x\n", empty_node_queue.tail);
		printf("tail->next: %x\n", empty_node_queue.tail->next);
		#endif
	}
	#ifdef DBG_SCHD
	printf("Dequeued node: %x\r\n", empty_node);
	#endif
	
	return empty_node;
}
