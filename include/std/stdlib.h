#ifndef __STDLIB_H__
#define __STDLIB_H__
#include <stddef.h>
#include <stdint.h>

typedef struct
{
	void*		location;
	uint32_t	used;
	size_t 		size;
} mem_hdr_t;

void* calloc(size_t nmemb, size_t size);
void* malloc(size_t size);
void free(void *ptr);
void* realloc(void *ptr, size_t size);

#endif