#include <kernel/res/ipc.h>
#include <kernel/mem/mem.h>
#include <string.h>
#include <kernel/proc/syscall.h>

k_ipc_port_tbl_t ipc_port_tbl;

void kipc_init()
{
    memset((char*)&ipc_port_tbl, 0, sizeof(k_ipc_port_tbl_t));
}

int kipc_register_port(uint32_t tid, p_addr_t addr, uint32_t port)
{
    //If the port number exceeds the maximum port id
    if(port>=IPC_PORTS_MAX)
        return -1;
    //Or if it's already registered to another process
    //Return immediately
    if(ipc_port_tbl.ports[port].loc)
        return -1;
    ipc_port_tbl.ports[port].owner_tid = tid;
    ipc_port_tbl.ports[port].loc = addr;
    return 0;
}

int kipc_unregister_port(uint32_t port)
{
    if(port>=IPC_PORTS_MAX)
        return -1;
    ipc_port_tbl.ports[port].owner_tid = 0;
    ipc_port_tbl.ports[port].loc = 0;
    return 0;
}

int kipc_map(v_addr_t target_addr, uint32_t port)
{
    if(port>=IPC_PORTS_MAX)
        return -1;
    if(!ipc_port_tbl.ports[port].loc)
        return -1;
    //mmap the appropriate processes page table
    (void)target_addr;
    //return -1, because currently not implemented
    return -1;
}
