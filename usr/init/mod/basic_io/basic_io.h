#ifndef _BASIC_IO_H__
#define _BASIC_IO_H__
void puthexbyte(char* dev, char byte);

void puthex(char* dev, unsigned int val);

void putc(int c, volatile char* dev);

void puts(char* s, volatile char* dev);

unsigned char getc(volatile char* dev);
#endif