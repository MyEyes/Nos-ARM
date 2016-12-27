#ifndef __IPC_H__
#define __IPC_H__
#include <stdint.h>
#include <stddef.h>
#include <lock.h>

#define MAX_NUM_IPC_PORTS 65535

typedef unsigned short port_id;

//Should never be passed as value, due to the buffer trick
typedef struct{
	port_id 	id;				//Unique id for this port
	size_t 		buffer_size;	//Size of buffer+sizeof(ipc_port) must be multiple of page size
	size_t		buffer_free;	//Currently free memory in buffer
	size_t		buffer_off_r;	//Current read offset into buffer
	size_t		buffer_off_w;	//Current write offset into buffer
	spinlock_t	lock;
	char 		buffer[];		//Buffer address start
} ipc_port_t;

//Should never be passed as value, due to the buffer trick
typedef struct{
	port_id		response_port; 	//If not 0 specifies a response port
	uint32_t	sender_thread; 	//If not 0 specifies the senders thread id
	size_t		length;			//Specifies the length of the message
	char		msg[];			//Start of the message buffer
} ipc_msg_t;

ipc_port_t*	ipc_create_port(port_id id, size_t buffer_size);
ipc_port_t*	ipc_bind_port(port_id id);
int			ipc_free_port(port_id id);

int     	ipc_read_msg(ipc_port_t* port, char* buffer, size_t max_size);
int			ipc_send_msg(ipc_port_t* port, port_id resp_port, size_t length, char* msg);

int rbuffer_read(char* buffer, size_t offset, size_t size, char* dst, size_t bytes);
int rbuffer_cpy(char* buffer, size_t offset, size_t size, char* src, size_t bytes);
#endif
