#include "gpio.h"

void __attribute__((naked)) gpio_delay(int32_t count)
{
	(void) count;
    __asm__ __volatile__(   "__delay_: subs r0, r0, #1\n"
                            "bne __delay_\n"
                            "bx lr");
}

//Memory offsets divided by 4 due to pointer arithmetic on int32

void gpio_set(volatile char* dev, char n)
{
    if(n<32)
        dev[GPIO_SET_1] = 1<<n;
    else
        dev[GPIO_SET_2] = 1<<(n-32);
}

void gpio_clr(volatile char* dev, char n)
{
    if(n<32)
        dev[GPIO_CLR_1] = 1<<n;
    else
        dev[GPIO_CLR_2] = 1<<(n-32);
}

void gpio_func_sel(volatile char* dev, char n, char val)
{
   char bank = n/10;
   uint32_t curr_val = dev[(GPIO_FSEL_0)+bank];
   uint32_t offset = n%10;
   uint32_t mask = (7<<(3*offset));
   curr_val &= ~mask;
   curr_val |= (uint32_t)val<<(3*offset);
   dev[GPIO_FSEL_0+bank] = curr_val;
}

char gpio_func_read(volatile char* dev, char n)
{
   char bank = n/10;
   uint32_t curr_val = dev[(GPIO_FSEL_0)+bank];
   uint32_t offset = n%10;
   return (curr_val>>(3*offset))&7;
}

void gpio_set_pullup(volatile char* dev, char val, uint32_t pins)
{
	dev[GPIO_GPPUD] = val;
	gpio_delay(200);
	dev[GPIO_GPPUDCLK0] = pins;
	gpio_delay(200);
	dev[GPIO_GPPUD] = 0;
	dev[GPIO_GPPUDCLK0] = 0;
}
