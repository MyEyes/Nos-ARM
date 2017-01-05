#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include __PLATFORM__

#include "kernel/mem/mem.h"
#include "kernel/mem/paging.h"

size_t min_alloc = 4096; //have to allocate at least page size

void* malloc(size_t size)
{
	mem_hdr_t* hdr = (mem_hdr_t*)mem_phys_find_free(size+sizeof(mem_hdr_t));
    mem_hdr_t* phdr = hdr;
    hdr = (mem_hdr_t*) TO_KERNEL_ADDR_SPACE(hdr);
	void* addr = ((char*)phdr) + sizeof(mem_hdr_t);
	if(hdr)
	{
		hdr->location = addr;
		hdr->size = size+sizeof(mem_hdr_t);
		hdr->used = 1;
	
		mem_phys_set(phdr, hdr->size);
		return (void*)TO_KERNEL_ADDR_SPACE(addr);
	}
	return 0;
}

void* realloc(void *ptr, size_t size)
{
	free(ptr);
	return malloc(size);
}

void* calloc(size_t nmemb, size_t size)
{
	void* ptr = malloc(nmemb*size);
	if(ptr)
	{
		memset(ptr, 0, nmemb*size);
	}
	return ptr;
}

void free(void *ptr)
{
	if(!ptr)
		return;
	mem_hdr_t *hdr = (mem_hdr_t*)((char*)ptr-sizeof(mem_hdr_t));
	if(hdr->location == ptr && hdr->used)
	{
		mem_phys_clear(hdr, hdr->size);
		hdr->used = 0;
		hdr->location = 0;
		hdr->size = 0;
	}
}
