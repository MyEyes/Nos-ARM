#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void putc(int c, volatile char* dev)
{
	while((dev[0x18] & (1 << 5))){ }
	*dev = c;
}

void puts(char* s, volatile char* dev)
{
	while(*s)
		putc(*s++, dev);
}

void main(uint32_t tid)
{
	(void) tid;
	std_init();
	int* test = malloc(sizeof(int));
	
	test[0]=tid;
	
	free(test);
	
	int* test_2 = malloc(sizeof(int));
	*test_2 = 32;
	
	volatile char* dev = req_dev("bcm2385_uart0");
	
	puts("Hellooooo from Usermode\r\n", dev);
	
	exit((int)dev);
}