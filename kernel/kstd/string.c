#include <string.h>

void memset(char* ptr, int c, size_t n)
{
	char* cp = ptr;
	for(;n>0;n--)
		cp[n-1] = (char)c;
}

void memcpy(char* trg, const char* src, size_t n)
{
	while(n>0)
	{
		*(trg++) = *(src++);
		n--;
	}
}

size_t strlen(const char* str)
{
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
}

int strcmp(const char* str1, const char* str2)
{
	while(*str1!=0 && *str1==*str2)
	{
		str1++;
		str2++;
	}
	return *str1-*str2;
}