#include <stdint.h>

void main(uint32_t tid)
{
	(void) tid;
	__asm__("swi 15");
	while(1);
}