#include "kernel/proc/proc.h"
#include "kernel/proc/thread.h"
#include "kernel/mem/mem.h"
#include "kernel/mem/paging.h"
#include "kernel/mem/perm.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include __PLATFORM__

proc_hdr_t kern_proc;

void proc_init(proc_hdr_t* proc, pg_tbl_t* tbl, uint32_t virt_end, uint32_t priv, uint32_t pid)
{
	proc->pid = pid;
	proc->pg_tbl = tbl;
	proc->priv = priv;
	proc->brk = virt_end;
}

pg_tbl_t* proc_create(char* virt_start, char* virt_end, uint32_t stack_size)
{
	pg_tbl_t* tbl = malloc(0x5000);
	
	if(!tbl)
		return (pg_tbl_t*)0;
	
	uint32_t stepsize = __plat_pg_tbl_maxentry();
	
	void* entry_loc = mem_phys_find_free(stepsize)+(uint32_t)PLATFORM_KERNEL_BASE;
	if(!entry_loc)
		return (pg_tbl_t*)0;
	
	mem_phys_set(entry_loc - (uint32_t)PLATFORM_KERNEL_BASE, stepsize);
	
	pg_create(tbl, entry_loc, (size_t)PLATFORM_PROC_MAX_MEM);
	
	uint32_t start_virt = ((uint32_t)virt_start)/stepsize;
	uint32_t end_virt = ((uint32_t)virt_end-1)/stepsize;
	uint32_t fail = 0;
	for(uint32_t i = start_virt; i<=end_virt && !fail; i++)
	{
		void* phys_addr = mem_phys_find_free(stepsize)+(uint32_t)PLATFORM_KERNEL_BASE;
		if(phys_addr)
		{
			//printf("mapping %x to %x\r\n", (char*)(i*stepsize), phys_addr);
			pg_map(tbl, (char*)(i*stepsize), phys_addr, stepsize, 0, PERM_PRW_URW, 0, 0, 0);
			mem_phys_set(phys_addr - (uint32_t)PLATFORM_KERNEL_BASE, stepsize);
		}
		else
		{
			fail = 1;
			break;
		}
	}
	
	uint32_t stack_end = (uint32_t)virt_start-stack_size;
	stack_end /= stepsize;
	for(uint32_t i=stack_end; i<start_virt && !fail; i++)
	{
		void* phys_addr = mem_phys_find_free(stepsize)+(uint32_t)PLATFORM_KERNEL_BASE;
		if(phys_addr)
		{
			//printf("mapping %x to %x\r\n", (char*)(i*stepsize), phys_addr);
			pg_map(tbl, (char*)(i*stepsize), phys_addr, stepsize, 0, PERM_PRW_URW, 0, 0, 0);
			mem_phys_set(phys_addr - (uint32_t)PLATFORM_KERNEL_BASE, stepsize);
		}
		else
		{
			fail = 1;
			break;
		}
	}
	
	if(fail)
		return (pg_tbl_t*)0;
	
	return tbl;
}

void* sbrk()
{
	int extra = (int)__plat_thread_getparam(curr_thread, 1);
	printf("sbrk called with parameter: %x\r\n", extra);
	void* obrk = (void*)curr_thread->proc->brk;
	//If we don't need to get or remove memory from the process
	if(!extra)
		return obrk;
	
	int32_t stepsize = __plat_pg_tbl_maxentry();
	uint32_t offset = 0;
	if(extra > 0)
	{
		while(extra>stepsize)
		{
			p_addr_t start = mem_phys_find_free(stepsize);
			if(!start)
				return (void*)-1;
			pg_map(curr_thread->proc->pg_tbl, (void*)(obrk+offset), start, stepsize, 0, PERM_PRW_URW, 0, 0, 0);
			extra-=stepsize;
			offset+=stepsize;
		}
		p_addr_t start = mem_phys_find_free(extra);
		pg_map(curr_thread->proc->pg_tbl, (void*)(obrk+offset), start, extra, 0, PERM_PRW_URW, 0, 0, 0);
		curr_thread->proc->brk += offset + extra;
	}
	else
	{
		while(extra<-stepsize)
		{
			pg_unmap(curr_thread->proc->pg_tbl, (void*)(obrk-offset-stepsize), stepsize);
			offset+=stepsize;
			extra+=stepsize;
		}
		pg_unmap(curr_thread->proc->pg_tbl,(void*)(obrk-offset+extra), -extra);
		curr_thread->proc->brk += extra - offset;
	}
	return obrk;
}