#include <kernel/res/res.h>
#include <kernel/mem/paging.h>
#include <kernel/mem/perm.h>
#include <kernel/proc/thread.h>
#include <kernel/res/ipc.h>
#include <ipc.h>

void res_create(res_t* res, uint32_t type, size_t size, void* addr)
{
    res->type = type;
    res->size = size;
    res->addr = addr;
    res_setDefaultHnd(res);
}

void res_setDefaultHnd(res_t* res)
{
    switch(res->type)
    {
        case RES_TYPE_UNKWN: break;
        case RES_TYPE_PMEM: res->grant = res_grnt_pmem;
                            res->remove = res_rm_pmem;
                            res->getName = res_gn_pmem;
                            res->reset = res_rs_pmem;
                            break;
        case RES_TYPE_PORT: res->grant = res_grnt_port;
                            res->remove = res_rm_port;
                            res->getName = res_gn_port;
                            res->reset = res_rs_port;
                            break;
        case RES_TYPE_MMDEV: res->grant = res_grnt_mmdev;
                             res->remove = res_rm_mmdev;
                             res->getName = res_gn_mmdev;
                             res->reset = res_rs_mmdev;
                             break;
    };
}

void res_grnt_pmem(res_t* res, v_addr_t addr)
{
    if(res->type!=RES_TYPE_PMEM)
        return;
    pg_map(curr_thread->proc->pg_tbl, addr, res->addr, res->size, 0, PERM_PRW_URW, 0, 1, 0);
}

void res_rm_pmem(res_t* res, v_addr_t addr)
{
    if(res->type!=RES_TYPE_PMEM)
        return;
    if(res->addr!=addr)
        return;
    pg_unmap(curr_thread->proc->pg_tbl, addr, res->size);
}

char* res_gn_pmem(res_t* res)
{
    if(res->type!=RES_TYPE_PMEM)
        return "INVAL";
    return "PMEM";
}

void res_rs_pmem(res_t* res)
{
    if(res->type!=RES_TYPE_PMEM)
        return;
    return;
}

void res_grnt_port(res_t* res, v_addr_t addr)
{
    if(res->type!=RES_TYPE_PORT)
        return;
    kipc_map(addr, (uint32_t)res->addr);
}

void res_rm_port(res_t* res, v_addr_t addr)
{
    if(res->type!=RES_TYPE_PORT)
        return;
    ipc_port_t* port = (ipc_port_t*) addr;
    pg_unmap(curr_thread->proc->pg_tbl, addr, port->buffer_size+sizeof(ipc_port_t));
}

char* res_gn_port(res_t* res)
{
    if(res->type!=RES_TYPE_PORT)
        return "INVAL";
    return "PORT";
}

void res_rs_port(res_t* res)
{
    if(res->type!=RES_TYPE_PORT)
        return;
    ipc_port_t* port = (ipc_port_t*) res->addr;
    //Unregister with 0 as tid, since kernel
    //always has permission and check should
    //happen earlier
    kipc_unregister_port(port->id, 0);
}

void res_grnt_mmdev(res_t* res, v_addr_t addr)
{
    if(res->type!=RES_TYPE_MMDEV)
        return; 
    pg_map(curr_thread->proc->pg_tbl, addr, res->addr, res->size, 0, PERM_PRW_URW, 0, 1, 0);
}

void res_rm_mmdev(res_t* res, v_addr_t addr)
{
    if(res->type!=RES_TYPE_MMDEV)
        return;
    if(res->addr!=addr)
        return;
    pg_unmap(curr_thread->proc->pg_tbl, res->addr, res->size);
}

char* res_gn_mmdev(res_t* res)
{
    if(res->type!=RES_TYPE_MMDEV)
        return "INVAL";
    return "MMDEV";
}

void res_rs_mmdev(res_t* res)
{
    if(res->type!=RES_TYPE_MMDEV)
        return;
    return;
}
