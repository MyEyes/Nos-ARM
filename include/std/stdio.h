#ifndef __STDIO_H__
#define __STDIO_H__
#include <stdarg.h>
#include <stdint.h>

typedef struct
{
	void (*putc)(unsigned char c);
} fileops_t;

typedef struct
{
	fileops_t ops;
} FILE;

int vprintf(const char *format, va_list arg);
int printf(const char *format, ...);
int fputc(int c, FILE* stream);

int print_hex(uint32_t u, FILE* stream);
void print_hexbyte(char byte, FILE* stream);

#endif