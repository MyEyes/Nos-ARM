#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

void main(uint32_t tid)
{
	(void) tid;
	char* test = sbrk(4096);
	test[1]='x';
	
	exit((int)test);
}