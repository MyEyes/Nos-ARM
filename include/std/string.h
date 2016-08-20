#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

void memset(char* ptr, int c, size_t size);
void memcpy(char* trg, const char* src, size_t n);
size_t strlen(const char* str);
#endif