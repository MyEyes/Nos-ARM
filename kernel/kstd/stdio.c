#include <stdio.h>
#include <string.h>
#include "kernel/mem/mem.h"
#include "kernel/mod/kernel_uart.h"

FILE stdout;

int printf(const char *format, ...)
{
	if(!format)
		return -1;
	va_list arg;
	int done;
	
	mem_dsb();
	mem_dmb();
	
	va_start(arg, format);
	done = vprintf(format, arg);
	va_end(arg);
	
	mem_dsb();
	mem_dmb();
	
	return done;
}

int vprintf(const char *format, va_list arg)
{
	uint32_t len = strlen(format);
		
	for(uint32_t i = 0; i<len; i++)
	{
		char c = format[i];
		if(!c)
			return 0;
		
		if(c=='%')
		{
			char f = format[++i];
			if(f=='x')
				print_hex(va_arg(arg, uint32_t), &stdout);
			else if(f=='s')
				printf(va_arg(arg, char*));
			else if(f=='c')
				fputc((char)va_arg(arg, uint32_t), &stdout);
		}
		else
			fputc(c, &stdout);
	}
	return 0;
}

int print_hex(uint32_t u, FILE* stream)
{	
	print_hexbyte(u>>24, stream);
	print_hexbyte((u>>16)&0xFF, stream);
	print_hexbyte((u>>8)&0xFF, stream);
	print_hexbyte(u&0xFF, stream);
	return 0;
}

void print_hexbyte(char byte, FILE* stream)
{
	char high = byte>>4;
	if(high<10)
		fputc(0x30+high, stream);
	else
		fputc(0x41+(high-10), stream);
	
	char low = byte&0xF;
	if(low<10)
		fputc(0x30+low, stream);
	else
		fputc(0x41+(low-10), stream);
}

int fputc(int c, FILE* stream)
{
	if(stream->ops.putc)
	{
		stream->ops.putc(c);
		return 0;
	}
	return -1;
}
