#include "std/stdlib.h"
#include "std/string.h"
#include "kernel/mem/mem.h"
#include <stdint.h>

typedef struct
{
	void*		location;
	uint32_t	used;
	size_t 		size;
} mem_hdr_t;

void* malloc(size_t size)
{
	mem_hdr_t* hdr = (mem_hdr_t*)mem_phys_find_free(size+sizeof(mem_hdr_t));
	void* addr = ((char*)hdr) + sizeof(mem_hdr_t);
	if(addr)
	{
		hdr->location = addr;
		hdr->size = size+sizeof(mem_hdr_t);
		hdr->used = 1;
	
		mem_phys_set(hdr, hdr->size);
	}
	return addr;
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