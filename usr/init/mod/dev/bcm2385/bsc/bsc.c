#include "bsc.h"

void i2c_enable(volatile i2c_register* dev)
{
    dev->control = 1<<I2C_ENABLE_OFF;
    dev->status = -1;
}

void i2c_start(volatile i2c_register* dev, uint32_t slave_addr, uint32_t data_len, char read)
{
    //Set slave addr and data length
    dev->slave_addr = slave_addr;
    dev->data_len = data_len;
    //Set or clear read flag
    if(read)
        dev->control |= 1<<I2C_READ_OFF;
    else
        dev->control &= ~(1<<I2C_READ_OFF);
	dev->status = -1;
    //dev->status |= 1<<I2C_CLKT_OFF | 1<<I2C_ERR_OFF | 1<<I2C_DONE_OFF;
    //Start transfer
    dev->control |= 1<<I2C_START_OFF | 1<<I2C_CLEAR_OFF;
}

int i2c_send(volatile i2c_register* dev, char data)
{
    //If we don't need to send more data
    if(!dev->data_len)
        return -1;
    //If there's no transfer going on right now or there was an error
    if(!(dev->status & (1<<I2C_TA_OFF)) || (dev->status & (1<<I2C_ERR_OFF)))
        return -1;
    //Spin until there is space in fifo
    while(!(dev->status & (1<<I2C_TXD_OFF)))
    {
        //timed out
        if(dev->status & (1<<I2C_CLKT_OFF))
        {
            //Clear timeout flag and return
            dev->status |= (1<<I2C_CLKT_OFF);
            return -1;
        }
        //Do nothing until i2c fifo has space for the byte
    }
    //Write data into fifo
    dev->data_fifo = data;
    return 0;
}

void i2c_clear(volatile i2c_register* dev)
{
    dev->control |= 1<<I2C_CLEAR_OFF;
}

void i2c_clear_err(volatile i2c_register* dev)
{
	dev->status |= 1<<I2C_ERR_OFF;
}
