#include <kernel/res/res_mgr.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <kernel/proc/syscall.h>
#include <kernel/proc/thread.h>
#include __PLATFORM__


size_t res_id_cnt = 0;
size_t res_tbl_size = 0;
res_hnd* res_tbl = (res_hnd*)0;

void* sys_res_rq_name();
void* sys_res_rq();

void res_mgr_init()
{
    res_tbl = (res_hnd*) malloc(sizeof(res_hnd)*RES_MGR_INIT_SIZE);
    if(!res_tbl)
        return;
    res_tbl_size = RES_MGR_INIT_SIZE;

    //Register ressource request syscalls
    syscall_set(SYSCALL_RES_RQ_NM, sys_res_rq_name);
    syscall_set(SYSCALL_RES_RQ, sys_res_rq);
}

res_id res_register(res_t* res, char* name)
{
    res_hnd* hnd = res_tbl_get_free();
    if(!hnd)
        return (res_id)-1;
    hnd->id=res_id_cnt++;
    hnd->name = name;
    hnd->res = res;
    return hnd->id;
}

res_hnd* res_req(res_id id)
{
    if(id<res_id_cnt)
        return res_tbl+id;
    return (res_hnd*)0;
}

res_hnd* res_req_nm(char* name)
{
    for(size_t x=0; x<res_id_cnt; x++)
    {
        if(!res_tbl[x].name)
            continue;
        if(strcmp(name, res_tbl[x].name)==0)
            return res_tbl+x;
    }
    return (res_hnd*)0;
}

res_hnd* res_tbl_get_free()
{
   if(res_id_cnt<res_tbl_size)
       return res_tbl+res_id_cnt;
   if(res_tbl_grw(res_tbl_size+(res_tbl_size>>1)))
       return (res_hnd*)0;
   return res_tbl+res_id_cnt;
}

int res_tbl_grw(size_t new_sz)
{
    res_hnd* newTab = (res_hnd*) malloc(sizeof(res_hnd)*new_sz);
    if(!newTab)
        return -1;
    memcpy((char*)newTab, (char*)res_tbl, res_tbl_size*sizeof(res_hnd));
    res_hnd* old = res_tbl;
    res_tbl = newTab;
    free(old);
    res_tbl_size = new_sz;
    return 0;
}

void res_tbl_dbg()
{
    printf("Res Tbl DBG\r\n");
    for(uint32_t x=0; x<res_id_cnt; x++)
    {
        res_hnd* hnd = res_tbl+x;
        char* name = hnd->name;
        if(!name)
            name="";
        if(!hnd->res || !hnd->res->getName)
        {
            printf("\t%x %x INVAL %s\r\n", x, hnd->res, name);
            continue;
        }
        printf("\t%x %s %x %x %s\r\n",hnd->id, hnd->res->getName(hnd->res), hnd->res->addr, hnd->res->size, name);
    }
}

void* sys_res_rq()
{
    res_id id = (res_id) __plat_thread_getparam(curr_thread, 1);
    v_addr_t addr = (v_addr_t)__plat_thread_getparam(curr_thread, 2);
    res_hnd* hnd = res_req(id);
    if(!hnd)
        return (void*)0;
    res_t* res=hnd->res;
    if(res->grant)
        res->grant(res, addr);
    else
        return (void*)0;
    return (void*) addr;
}

void* sys_res_rq_name()
{
    printf("Doing res_rq_nm!\r\n");
    char* res_name = (char*)__plat_thread_getparam(curr_thread, 1);
    v_addr_t addr = (v_addr_t)__plat_thread_getparam(curr_thread, 2);
    res_hnd* hnd = res_req_nm(res_name);
    if(!hnd)
        return (void*)0;
    res_t* res = hnd->res;
    if(res->grant)
        res->grant(res, addr);
    else
        return (void*)0;
    return (void*)addr;
}
