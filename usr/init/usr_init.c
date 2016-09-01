#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void main(uint32_t tid)
{
	(void) tid;
	char* dev = req_dev("bcm2385_uart0");
	
	//Wait for uart to be ready
	while(dev[0x18] & (1 << 5));
	*dev = 'A';
	
	exit((int)dev);
}