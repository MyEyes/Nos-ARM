#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>
#include __PLATFORM__
#include "kernel/mem/mem.h"
#include "kernel/module.h"
#include "kernel/mod/kernel_uart.h"
#include "std/stdio.h"

extern FILE stdout;

static char* kuart_dev = "bcm2385_uart";

static module_hdr_t kuart_hdr=
{
	.magic = MOD_MAGIC,
	.type = SRV,
	.mod_name = "bcm2385_uart",
	.srv_name = "uart",
	.num_devs = 1,
	.dev_lst = &kuart_dev,
	.num_reqs = 0,
	.init = uart_mod_init,
	.req_list = (char**) 0
};
 
inline void mmio_write(uint32_t reg, uint32_t data)
{
	*(volatile uint32_t *)reg = data;
}
 
inline uint32_t mmio_read(uint32_t reg)
{
	return *(volatile uint32_t *)reg;
}
 
/* Loop <delay> times in a way that the compiler won't optimize away. */

inline void delay(int32_t count)
{
	asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
		 : "=r"(count): [count]"0"(count) : "cc");
}
 
size_t strlen(const char* str)
{
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
}

void uart_mod_init(void* addr, uint32_t port)
{
	//Remove unused variable warning
	(void) kuart_hdr;
	(void) addr;
	(void) port;
	uart_init();
	
	stdout.ops.putc = uart_putc;
}

void uart_init()
{
	
	// Disable UART0.
	mmio_write(UART0_CR, 0x00000000);
	// Setup the GPIO pin 14 && 15.
 
	// Disable pull up/down for all GPIO pins & delay for 150 cycles.
	mmio_write(GPPUD, 0x00000000);
	delay(150);
 
	// Disable pull up/down for pin 14,15 & delay for 150 cycles.
	mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
	delay(150);
 
	// Write 0 to GPPUDCLK0 to make it take effect.
	mmio_write(GPPUDCLK0, 0x00000000);
 
	// Clear pending interrupts.
	mmio_write(UART0_ICR, 0x7FF);
 
	// Set integer & fractional part of baud rate.
	// Divider = UART_CLOCK/(16 * Baud)
	// Fraction part register = (Fractional part * 64) + 0.5
	// UART_CLOCK = 3000000; Baud = 115200.
 
	// Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
	// Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
	mmio_write(UART0_IBRD, 1);
	mmio_write(UART0_FBRD, 40);
 
	// Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
	mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
 
	// Mask all interrupts.
	mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
	                       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
 
	// Enable UART0, receive & transfer part of UART.
	mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}
 
void uart_putc(unsigned char byte)
{
	// Wait for UART to become ready to transmit.
	while ( mmio_read(UART0_FR) & (1 << 5) ) { }
	mmio_write(UART0_DR, byte);
}

void uart_puthex(unsigned int val)
{
	uart_puthexbyte(val>>24);
	uart_puthexbyte((val>>16)&0xFF);
	uart_puthexbyte((val>>8)&0xFF);
	uart_puthexbyte(val&0xFF);
}

void uart_puthexbyte(char byte)
{
	char high = byte>>4;
	if(high<10)
		uart_putc(0x30+high);
	else
		uart_putc(0x41+(high-10));
	
	char low = byte&0xF;
	if(low<10)
		uart_putc(0x30+low);
	else
		uart_putc(0x41+(low-10));
}
 
unsigned char uart_getc()
{
    // Wait for UART to have recieved something.
    while ( mmio_read(UART0_FR) & (1 << 4) ) { }
    return mmio_read(UART0_DR);
}
 
void uart_write(const unsigned char* buffer, size_t size)
{
	for ( size_t i = 0; i < size; i++ )
		uart_putc(buffer[i]);
}
 
void uart_puts(const char* str)
{
	uart_write((const unsigned char*) str, strlen(str));
}