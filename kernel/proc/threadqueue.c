#include "kernel/proc/threadqueue.h"
#include "std/stdlib.h"
#include <stdint.h>

void enqueue_node(thread_queue_t* queue, thread_node_t* node, thread_t* thread) 
{
	node->data = thread;
	node->next = 0;
	
	if(!queue->head) 
	{
		queue->head = queue->tail = node;
		return;
	}
	
	queue->tail->next = node;
	queue->tail = node;
}

thread_node_t* dequeue_node(thread_queue_t* queue) 
{
	thread_node_t* dequeued_node = queue->head;
	
	if(dequeued_node == queue->tail) 
	{
		queue->head = queue->tail = 0;
	}
	else
	{
		queue->head = queue->head->next;
	}
	
	return dequeued_node;
}