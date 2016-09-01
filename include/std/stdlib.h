#ifndef __STDLIB_H__
#define __STDLIB_H__
#include <stddef.h>
#include <stdint.h>

extern size_t min_alloc;

typedef struct
{
	void*		location;
	uint32_t	used;
	size_t 		size;
} mem_hdr_t;

void std_init();
void* calloc(size_t nmemb, size_t size);
void* malloc(size_t size);
void free(void *ptr);
void* realloc(void *ptr, size_t size);

void abort();
void exit(int status);
void system(const char*);

#endif