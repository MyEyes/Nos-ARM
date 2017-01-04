#include "gpio.h"

//Memory offsets divided by 4 due to pointer arithmetic on int32

void gpio_set(uint32_t* dev, char n)
{
    if(n<32)
        dev[GPIO_SET_1>>2] = 1<<n;
    else
        dev[GPIO_SET_2>>2] = 1<<(n-32);
}

void gpio_clr(uint32_t* dev, char n)
{
    if(n<32)
        dev[GPIO_CLR_1>>2] = 1<<n;
    else
        dev[GPIO_CLR_2>>2] = 1<<(n-32);
}

void gpio_func_sel(uint32_t* dev, char n, char val)
{
   char bank = n/10;
   uint32_t curr_val = dev[(GPIO_FSEL_0>>2)+bank];
   uint32_t offset = n%10;
   uint32_t mask = (7<<(3*offset));
   curr_val &= ~mask;
   curr_val |= (uint32_t)val<<(3*offset);
   dev[GPIO_FSEL_0+bank] = curr_val;
}

char gpio_func_read(uint32_t* dev, char n)
{
   char bank = n/10;
   uint32_t curr_val = dev[(GPIO_FSEL_0>>2)+bank];
   uint32_t offset = n%10;
   return (curr_val>>(3*offset))&7;
}