
ipc_create_port(int port_id, size_t buffer_size)
{
	size_t port_size = sizeof(ipc_port_t) + buffer_size - 1; //Subtracting 1, since the ipc_port_t contains a char buffer[1] hack
	void* phys_loc = mem_phys_find_free(port_size);
}