#include <ipc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "kernel/proc/syscall.h"
#include __PLATFORM__

ipc_port_t* ipc_create_port(port_id id, size_t buffer_size)
{
    int res = 0;
    PLATFORM_SYSCALL2(SYSCALL_IPC_REG_PRT, id, buffer_size, res);
    if(res)
        return (ipc_port_t*)-1;
    ipc_port_t* port = ipc_bind_port(id);
    if(port==(ipc_port_t*)-1)
        return port;
    port->id = id;
    port->buffer_size = buffer_size-sizeof(ipc_port_t);
    port->buffer_free = buffer_size-sizeof(ipc_port_t);
    port->buffer_off_r = 0;
    port->buffer_off_w = 0;
    port->lock = 0;
    return port;
}

ipc_port_t* ipc_bind_port(port_id id)
{
    int res = 0;
    v_addr_t buffer = (v_addr_t)0x81000;
	PLATFORM_SYSCALL2(SYSCALL_IPC_MAP_PRT, id, buffer, res);
    if(res)
        return (ipc_port_t*)-1;
    return (ipc_port_t*)buffer;
}

int ipc_free_port(port_id id)
{
    int res = -1;
    PLATFORM_SYSCALL1(SYSCALL_IPC_UNREG_PRT, id, res);
	return res;
}

int ipc_read_msg(ipc_port_t* port, char* buffer, size_t max_size)
{
    if(max_size<sizeof(ipc_msg_t))
        return -1;

    spinlock_lock(&(port->lock));

    //Check that the buffer isn't empty before attempting to read a message
    if(port->buffer_size>port->buffer_free)
    {
        //Read message header into buffer
        rbuffer_read(port->buffer, port->buffer_off_r, port->buffer_size, buffer, sizeof(ipc_msg_t));
        //Get header info
        ipc_msg_t* msg = (ipc_msg_t*)buffer;
        //If the whole message doesn't exist in the header
        if(port->buffer_size-port->buffer_free<sizeof(ipc_msg_t)+msg->length)
        {
            spinlock_unlock(&(port->lock));
            return -1;
        }
        //Read remainder of message
        rbuffer_read(port->buffer, port->buffer_off_r+sizeof(ipc_msg_t), port->buffer_size, buffer+sizeof(ipc_msg_t), msg->length);
        //Correct read offset in buffer
        port->buffer_off_r += sizeof(ipc_msg_t)+msg->length;
        if(port->buffer_off_r >= port->buffer_size)
            port->buffer_off_r -= port->buffer_size;
        port->buffer_free += msg->length+sizeof(ipc_msg_t);
    }
    spinlock_unlock(&(port->lock));
	return 0;
}

int rbuffer_read(char* buffer, size_t offset, size_t buffer_size, char* dst, size_t bytes)
{
   if(bytes>buffer_size)
        return -1;

   if(offset+bytes<buffer_size)
       memcpy(dst, buffer+offset, bytes);
   else
   {
       memcpy(dst, buffer+offset, buffer_size-offset);
       memcpy(dst+buffer_size-offset, buffer, bytes-buffer_size+offset);
   }

   return 0;
}

//helper function to copy to a ringbuffer
int rbuffer_cpy(char* buffer, size_t offset, size_t buffer_size, char* src, size_t bytes)
{
    if(bytes>buffer_size)
        return -1;
    if(offset+bytes<buffer_size)
        memcpy(buffer+offset, src, bytes);
    else
    {
        memcpy(buffer+offset, src, buffer_size-offset);
        memcpy(buffer, src+buffer_size-offset ,bytes-buffer_size+offset);
    }
    return 0;
}

int	ipc_send_msg(ipc_port_t* port, port_id resp_port, size_t length, char* msg)
{
	(void)resp_port;
	//acquire lock
	spinlock_lock(&(port->lock));
	
	//Check that message will fit into buffer
	//We subtract 1 because the buffer[1] trick causes one extra byte to be allocated
	//If it doesn't fit return -1
	size_t total_size = sizeof(ipc_msg_t)+length;
	if(port->buffer_free<total_size)
	{
		spinlock_unlock(&(port->lock));
		return -1;
	}

    ipc_msg_t hdr;
    hdr.response_port = resp_port;
    hdr.sender_thread = get_pid();
    hdr.length = length;

    //Copy header into buffer
    //Exit on fail
    if(rbuffer_cpy(port->buffer, port->buffer_off_w, port->buffer_size, (char*)&hdr, sizeof(ipc_msg_t)))
    {
        spinlock_unlock(&(port->lock));
        return -1;
    }

    //Copy message to ringbuffer
    //Exit on fail
    if(rbuffer_cpy(port->buffer, port->buffer_off_w+sizeof(ipc_msg_t), port->buffer_size, msg, length))
    {
        spinlock_unlock(&(port->lock));
        return -1;
    }
	
	//Reduce free space in buffer
	port->buffer_free -= total_size;
    port->buffer_off_w += total_size;
	
	//If we would write over the end of the ringbuffer
	//we instead write to the end and set up the values so that we only write the remainder
	//at the start of the ringbuffer
	if(port->buffer_off_w+length>=port->buffer_size)
	{
        //If we wrote over the end we adjust the offset to
        //be within bounds again
		port->buffer_off_w -= port->buffer_size;
	}
	
	spinlock_unlock(&(port->lock));
	return 0;
}
