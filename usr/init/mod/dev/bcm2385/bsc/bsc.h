#ifndef _BSC_H__
#define _BSC_H__
#include <stdint.h>

//I2C CTRL OFFSETS
#define I2C_ENABLE_OFF 15
#define I2C_START_OFF 7
#define I2C_CLEAR_OFF 4
#define I2C_READ_OFF 0

//I2C STATUS OFFSETS
#define I2C_TXD_OFF 4
#define I2C_TA_OFF 0
#define I2C_CLKT_OFF 9
#define I2C_ERR_OFF 8
#define I2C_DONE_OFF 1

typedef struct
{
    volatile uint32_t control;
    volatile uint32_t status;
    volatile uint32_t data_len;
    volatile uint32_t slave_addr;
    volatile uint32_t data_fifo;
    volatile uint32_t clk_div;
    volatile uint32_t data_delay;
    volatile uint32_t clk_stretch;
} i2c_register;

void i2c_enable(i2c_register* dev);
void i2c_start(i2c_register* dev, uint32_t slave_addr, uint32_t data_len, char read);
int i2c_send(i2c_register* dev, char data);
void i2c_clear(i2c_register* dev);
#endif
