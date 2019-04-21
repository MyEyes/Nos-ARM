#define GPIO_SET_1 0x1C
#define GPIO_SET_2 0x20
#define GPIO_CLR_1 0x28
#define GPIO_CLR_2 0x2C
#define GPIO_FSEL_0 0x00

#define GPIO_FUNC_IN 0
#define GPIO_FUNC_OUT 1

#define GPIO_LEVEL_1 0x34
#define GPIO_LEVEL_2 0x38

#define GPIO_GPPUD 0x94
#define GPIO_GPPUDCLK0 0x98
#define GPIO_GPPUDCLK1 0x9C

#include <stdint.h>

void gpio_set(volatile char* dev, char n);

void gpio_clr(volatile char* dev, char n);

void gpio_func_sel(volatile char* dev, char n, char val);

char gpio_func_read(volatile char* dev, char n);

void gpio_set_pullup(volatile char* dev, char val, uint32_t pins);
