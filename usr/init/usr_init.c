#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

void main(uint32_t tid)
{
	(void) tid;
	void* test = sbrk(0);
	exit((int)test);
}