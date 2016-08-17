#ifndef __STDLIB_H__
#define __STDLIB_H__
#include <stddef.h>

void* calloc(size_t nmemb, size_t size);
void* malloc(size_t size);
void free(void *ptr);
void* realloc(void *ptr, size_t size);

#endif