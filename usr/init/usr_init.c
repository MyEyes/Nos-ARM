#include <stdint.h>

void main(uint32_t tid)
{
	(void) tid;
	while(1)__asm__("swi 15");
}