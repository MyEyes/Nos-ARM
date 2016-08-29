#include <stdlib.h>
#include "usr/std/memory.h"
#include "std/unistd.h"
#include <stdint.h>

mem_area_t* head = 0;
mem_area_t* tail = 0;

void mem_init()
{
	uint32_t p_break = (uint32_t)sbrk(4096); //guarantee at least 4KB can be used by the process
	head = (mem_area_t*)p_break;
	tail = (mem_area_t*)p_break;
	head->next = 0;
	head->prev = 0;
	head->size = 4096;
	head->free = 1;
}

mem_area_t* mem_find_free_area(size_t size)
{
	size = MEM_ALIGN_BY_4(size);
	
	mem_area_t* curr_area = head;
	
	//loop through all previously allocated memory to try and find a free space
	do
	{
		if((curr_area->size - MEM_HEADER_SIZE >= size) && curr_area->free)
			return curr_area;
		
		curr_area = curr_area->next;
	} while (curr_area);
		
	return 0;
}

mem_area_t* mem_split_area(mem_area_t* area, size_t size)
{
	size = MEM_ALIGN_BY_4(size);
	
	if (size + MEM_HEADER_SIZE >= area->size)
		return area;
	
	size_t total_size = area->size; 								//store the total size of the area we're splitting
	area->size = size + MEM_HEADER_SIZE;							//partition the first area to be as big as what we want
	mem_area_t* new_area = (mem_area_t*)((char*)area + area->size); //create a second area to be the second partition with the remaining size
	
	new_area->size = total_size - area->size;
	new_area->next = area->next;
	new_area->prev = area;
	area->next = new_area;
	new_area->free = area->free;
	
	if (area == tail)
		tail = new_area;
	
	return new_area;
}

void mem_join_area(mem_area_t* area)
{
	if(area->next && area->next->free)
	{
		area->size = area->size + area->next->size;
		if(area->next == tail)
			tail = area;
		
		area->next = area->next->next;
	}
	else if(area->prev && area->prev->free)
	{
		area->prev->size = area->size + area->prev->size;
		if(area == tail)
			tail = area->prev;
		
		area->prev->next = area->next->next;
	}
}

mem_area_t* mem_extend_heap(size_t size) 
{
	size = MEM_ALIGN_BY_4(size);
	//the new area goes at the end of the tail pointer
	mem_area_t* new = (mem_area_t*)((char*)tail + tail->size);
	
	if (sbrk(size + MEM_HEADER_SIZE) == (void*)-1)
		return 0;
	
	new->next = 0;
	new->prev = tail;
	new->size = size;
	new->free = 1;
	tail->next = new;
	tail = new;
	
	return new;
}

