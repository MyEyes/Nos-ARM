#include "std/string.h"

void memset(char* ptr, int c, size_t n)
{
	char* cp = ptr;
	for(;n>0;n--)
		cp[n-1] = (char)c;
}


size_t strlen(const char* str)
{
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
}