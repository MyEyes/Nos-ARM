#include <ipc.h>
#include <stddef.h>
#include <kernel/mem/mem.h>
ipc_port_t* ipc_create_port(port_id port_id, size_t buffer_size)
{
	(void)port_id;
	size_t port_size = sizeof(ipc_port_t) + buffer_size - 1; //Subtracting 1, since the ipc_port_t contains a char buffer[1] hack
	void* phys_loc = mem_phys_find_free(port_size);
	(void)phys_loc;
	return (ipc_port_t*)0;
}
