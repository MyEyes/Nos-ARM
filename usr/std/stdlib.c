#include <stdlib.h>
#include "std/string.h"
#include <kernel/proc/syscall.h>
#include "usr/std/memory.h"
#include __PLATFORM__

void* calloc(size_t nmemb, size_t size)
{
	mem_area_t* alloc_area = mem_find_free_area(size*nmemb);
	memset((char*)(alloc_area + MEM_HEADER_SIZE), 0, (size*nmemb - MEM_HEADER_SIZE));
	return alloc_area;
}

void* malloc(size_t size)
{
	//size of 0 is defined to return a null pointer
	if (!size)
		return 0;
	
	mem_area_t* alloc_area = mem_find_free_area(size);
	
	//if we weren't able to find a free space, extend the heap to get some memory
	//otherwise, split up the space so you don't take more than you need
	if(!alloc_area)
		alloc_area = mem_extend_heap(size);
	else
		mem_split_area(alloc_area, size);
	
	alloc_area->free = 0;
	
	return (void*)alloc_area;
}

void free(void *ptr)
{
	if(!ptr)
		return;
	
	//try to add it to later segments of memory, and set it to freed
	mem_join_area((mem_area_t*)ptr);
	((mem_area_t*)ptr)->free = 1;
}

void* realloc(void *ptr, size_t size)
{	
	free(ptr);
	void *ret_ptr = malloc(size);
	return ret_ptr;
}

void __attribute__((noreturn)) abort()
{
	int return_val = 0;
	PLATFORM_SYSCALL1(SYSCALL_THREAD_TERM, -1, return_val);
	while(1);
}

void __attribute__((noreturn)) exit(int status)
{
	int return_val = 0;
	PLATFORM_SYSCALL1(SYSCALL_THREAD_EXIT, status, return_val);
	while(1);
}

void system(const char* call)
{
	(void)call;
}