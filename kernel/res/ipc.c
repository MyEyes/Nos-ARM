#include <kernel/res/ipc.h>
#include <kernel/mem/mem.h>
#include <kernel/mem/perm.h>
#include <string.h>
#include <stdio.h>
#include <kernel/proc/syscall.h>
#include <kernel/proc/schd.h>
#include __PLATFORM__

k_ipc_port_tbl_t ipc_port_tbl;

void kipc_init()
{
    memset((char*)&ipc_port_tbl, 0, sizeof(k_ipc_port_tbl_t));
    syscall_set(SYSCALL_IPC_REG_PRT, sys_kipc_reg_port);
    syscall_set(SYSCALL_IPC_UNREG_PRT, sys_kipc_unreg_port);
    syscall_set(SYSCALL_IPC_MAP_PRT, sys_kipc_map_port);
}

int sys_kipc_reg_port()
{
    size_t  buffer_sz = (size_t)__plat_thread_getparam(curr_thread, 2);
    uint32_t port = __plat_thread_getparam(curr_thread, 1);
    printf("Trying to register port %x with %x memory\r\n", port, buffer_sz);
    return kipc_register_port(curr_thread->tid, buffer_sz, port);
}

int sys_kipc_unreg_port()
{
    uint32_t port = __plat_thread_getparam(curr_thread, 1);
    return kipc_unregister_port(port, curr_thread->tid);
}

int sys_kipc_map_port()
{
    v_addr_t addr = (v_addr_t) __plat_thread_getparam(curr_thread, 2);
    uint32_t port = __plat_thread_getparam(curr_thread, 1);
    printf("Trying to map %x to port %x\r\n", addr, port);
    return kipc_map(addr, port);
}

int kipc_register_port(uint32_t tid, size_t size, uint32_t port)
{
    //If the port number exceeds the maximum port id
    if(port>=IPC_PORTS_MAX)
        return -1;
    //Or if it's already registered to another process
    //Return immediately
    if(ipc_port_tbl.ports[port].loc)
    {
        printf("Port already taken\r\n");
        return -1;
    }

    p_addr_t addr = (p_addr_t) mem_phys_find_free(size);
    if(!addr)
        return -1;
    mem_phys_set(addr, size);

    ipc_port_tbl.ports[port].owner_tid = tid;
    ipc_port_tbl.ports[port].loc = addr;
    return 0;
}

int kipc_unregister_port(uint32_t port, uint32_t tid)
{
    //Make sure we are mapping a port that exists
    //And that we are either the owner or the kernel
    if(port>=IPC_PORTS_MAX || (ipc_port_tbl.ports[port].owner_tid!=tid && tid!=0))
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
    return pg_map(curr_thread->proc->pg_tbl, target_addr, ipc_port_tbl.ports[port].loc, 0x1000, 0, PERM_PRW_URW, 0, 1, 0);
}
