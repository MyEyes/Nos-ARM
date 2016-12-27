#include <stdlib.h>
#include "std/string.h"
#include <kernel/proc/syscall.h>
#include "usr/std/memory.h"
#include __PLATFORM__

void std_init()
{
	mem_init();
}

uint32_t get_pid()
{
    return 1;
}

void* calloc(size_t nmemb, size_t size)
{
	size_t total_size = MEM_ALIGN_BY_4(nmemb*size);
	mem_area_t* alloc_area = malloc(total_size);
	memset((char*)(alloc_area), 0, total_size);
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
	
	return (void*)alloc_area + MEM_HEADER_SIZE;
}

void free(void *ptr)
{
	if(!ptr)
		return;
	
	//try to add it to later segments of memory, and set it to freed
	mem_join_area(((mem_area_t*)ptr) - 1); //subtract the size of the header
	(((mem_area_t*)ptr) - 1)->free = 1;
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
