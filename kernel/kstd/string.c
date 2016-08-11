#include "std/string.h"

void memset(void* ptr, int c, size_t n)
{
	char* cp = (char*)ptr;
	for(;n>0;n--)
		cp[n-1] = (char)c;
}