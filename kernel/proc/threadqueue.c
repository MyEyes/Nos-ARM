#include "kernel/proc/threadqueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

void enqueue_node(thread_queue_t* queue, thread_node_t* node, thread_t* thread) 
{
	//printf("Enqueueing %x\r\n", node);
	node->data = thread;
	node->next = 0;
	
	if(!queue->head) 
	{
		queue->head = queue->tail = node;
		return;
	}
	
	queue->tail->next = node;
	queue->tail = node;
	//printf("Head: %x\r\n", queue->head);
}

thread_node_t* dequeue_node(thread_queue_t* queue) 
{
	//printf("Dequeue Head: %x->%x\r\n", queue, queue->head);
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