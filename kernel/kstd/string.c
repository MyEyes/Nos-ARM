#include "std/string.h"

void memset(char* ptr, int c, size_t n)
{
	char* cp = ptr;
	for(;n>0;n--)
		cp[n-1] = (char)c;
}