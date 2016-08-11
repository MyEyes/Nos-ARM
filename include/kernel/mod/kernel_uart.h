#ifndef __KERNEL_UART_H__
#define __KERNEL_UART_H__

#include <stddef.h>
#include <stdint.h>
#include __PLATFORM__
 
void mmio_write(uint32_t reg, uint32_t data);
 
uint32_t mmio_read(uint32_t reg);
 
/* Loop <delay> times in a way that the compiler won't optimize away. */
void delay(int32_t count);
 
size_t strlen(const char* str);
 
enum
{
    // The GPIO registers base address.
    GPIO_BASE = GPIO_BASE_ADDR,
 
    // The offsets for reach register.
 
    // Controls actuation of pull up/down to ALL GPIO pins.
    GPPUD = (GPIO_BASE + 0x94),
 
    // Controls actuation of pull up/down for specific GPIO pin.
    GPPUDCLK0 = (GPIO_BASE + 0x98),
 
    // The base address for UART.
    UART0_BASE = UART0_BASE_ADDR,
 
    // The offsets for reach register for the UART.
    UART0_DR     = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR     = (UART0_BASE + 0x18),
    UART0_ILPR   = (UART0_BASE + 0x20),
    UART0_IBRD   = (UART0_BASE + 0x24),
    UART0_FBRD   = (UART0_BASE + 0x28),
    UART0_LCRH   = (UART0_BASE + 0x2C),
    UART0_CR     = (UART0_BASE + 0x30),
    UART0_IFLS   = (UART0_BASE + 0x34),
    UART0_IMSC   = (UART0_BASE + 0x38),
    UART0_RIS    = (UART0_BASE + 0x3C),
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44),
    UART0_DMACR  = (UART0_BASE + 0x48),
    UART0_ITCR   = (UART0_BASE + 0x80),
    UART0_ITIP   = (UART0_BASE + 0x84),
    UART0_ITOP   = (UART0_BASE + 0x88),
    UART0_TDR    = (UART0_BASE + 0x8C),
};

void uart_mod_init(void*, uint32_t);
 
void uart_init();
 
void uart_putc(unsigned char byte);

void uart_puthex(unsigned int val);
void uart_puthexbyte(char byte);
 
unsigned char uart_getc();
 
void uart_write(const unsigned char* buffer, size_t size);
 
void uart_puts(const char* str);
#endif