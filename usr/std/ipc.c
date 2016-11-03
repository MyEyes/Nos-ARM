#include <ipc.h>
#include <stdlib.h>
#include <string.h>

ipc_port_t* ipc_create_port(port_id id, size_t buffer_size)
{
	
}

ipc_port_t* ipc_bind_port(port_id id)
{
	
}

int ipc_free_port(ipc_port_t* port)
{
	
}

ipc_msg_t* ipc_read_msg(ipc_port_t* port)
{
	
}

//helper function to copy to a ringbuffer
void __attribute__((inline)) rbuffer_cpy(char* buffer, size_t offset, size_t buffer_size, char* src, size_t bytes)
{
	
}

int	ipc_send_msg(ipc_port_t* port, port_id resp_port, size_t length, char* msg)
{
	//acquire lock
	spinlock_lock(&(port->lock));
	
	//Check that message will fit into buffer
	//We subtract 1 because the buffer[1] trick causes one extra byte to be allocated
	//If it doesn't fit return -1
	size_t total_size = sizeof(ipc_msg_t)+length-1;
	if(port->buffer_free<total_size)
	{
		spinlock_unlock(&(port->lock));
		return -1;
	}
	
	//Reduce free space in buffer
	port->buffer_free -= length;
	
	//If we would write over the end of the ringbuffer
	//we instead write to the end and set up the values so that we only write the remainder
	//at the start of the ringbuffer
	if(port->buffer_off_w+length>=port->buffer_size)
	{
		//Figure out how many bytes to write before the end of the buffer
		size_t ilength = port->buffer_size - port->buffer_off_w - 1;
		//Copy until end of buffer
		memcpy(port->buffer + port->buffer_off_w, msg, ilength);
		//Adjust length, offset and msg start to copy
		length -= ilength;
		port->buffer_off_w = 0;
		msg += ilength;
	}
	
	//Copy message into buffer at current offset
	memcpy(port->buffer + port->buffer_off_w, msg, length);
	
	//Move buffer offset
	port->buffer_off_w += length;
	
	spinlock_unlock(&(port->lock);
}