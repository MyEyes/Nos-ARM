#include "basic_io.h"


void puthexbyte(char* dev, char byte)
{
	char high = byte>>4;
	if(high<10)
		putc(0x30+high, dev);
	else
		putc(0x41+(high-10), dev);
	
	char low = byte&0xF;
	if(low<10)
		putc(0x30+low, dev);
	else
		putc(0x41+(low-10), dev);
}


void puthex(char* dev, unsigned int val)
{
	puthexbyte(dev, val>>24);
	puthexbyte(dev, (val>>16)&0xFF);
	puthexbyte(dev, (val>>8)&0xFF);
	puthexbyte(dev, val&0xFF);
}

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