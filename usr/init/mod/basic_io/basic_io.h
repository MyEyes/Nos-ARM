#ifndef _BASIC_IO_H__
#define _BASIC_IO_H__
void puthexbyte(volatile char* dev, char byte);

void puthex(volatile char* dev, unsigned int val);

void putc(char c, volatile char* dev);

void puts(char* s, volatile char* dev);

unsigned char getc(volatile char* dev);
#endif
