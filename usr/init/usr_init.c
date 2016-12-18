#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void putc(int c, volatile char* dev)
{
	//Wait until device ready
	//This only works for the bcm2385_uart0
	//So there's clearly some work left to be done
	while((dev[0x18] & (1 << 5))){ }
	*dev = c;
}

void puts(char* s, volatile char* dev)
{
	while(*s)
		putc(*s++, dev);
}

unsigned char getc(volatile char* dev)
{
    // Wait for UART to have recieved something.
    while ( dev[0x18] & (1 << 4) ) { }
    return dev[0];
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
	
	char* test3 = (void*)0xFFFFFF00;
	*test3 = 'a';
	
	volatile char* dev = req_dev("bcm2385_uart0");
	
    volatile char* dev2 = (char*) pmap((void*)0x20201000, (void*)0x80000, 0x1000);
	(void)dev2;
	
	puts("Hellooooo from Usermode\r\n", dev2);
	char c;
	char input[512];
	unsigned short position = 0;
	char quit = 0;
	while(!quit)
	{
		while((c = getc(dev))!='\n')
		{
			putc(c, dev);
			input[position++] = c;
		}
		input[position++] = 0;
		puts("\n", dev);
		position = 0;
		if(!strcmp(input, "quit"))
			quit = 1;
	}
	
	exit((int)dev);
}
