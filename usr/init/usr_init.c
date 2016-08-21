#include <stdint.h>

void main(uint32_t tid)
{
	(void) tid;
	__asm__("udf 0");
	while(1);
}