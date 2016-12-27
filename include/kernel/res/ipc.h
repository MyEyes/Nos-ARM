#ifndef __K_IPC_H__
#define __K_IPC_H__
#define IPC_PORTS_MAX 4096

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
typedef struct{
    uint32_t    owner_tid;
    p_addr_t      loc;
} k_ipc_port_t;

typedef struct{
    k_ipc_port_t ports[IPC_PORTS_MAX];
} k_ipc_port_tbl_t;

void kipc_init();
int kipc_register_port(uint32_t tid, size_t size, uint32_t port);
int kipc_unregister_port(uint32_t port, uint32_t tid);
int kipc_map(v_addr_t target_addr, uint32_t port);

int sys_kipc_reg_port();
int sys_kipc_unreg_port();
int sys_kipc_map_port();
#endif
