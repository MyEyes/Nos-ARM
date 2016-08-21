#include "kernel/proc/proc.h"
#include "kernel/mem/mem.h"
#include "kernel/mem/paging.h"
#include "kernel/mem/perm.h"
#include "std/stdlib.h"
#include "std/stdio.h"
#include __PLATFORM__

proc_hdr_t kern_proc;

void proc_init(proc_hdr_t* proc, pg_tbl_t* tbl, uint32_t priv, uint32_t pid)
{
	proc->pid = pid;
	proc->pg_tbl = tbl;
	proc->priv = priv;
}

pg_tbl_t* proc_create(char* virt_start, char* virt_end, uint32_t stack_size)
{
	pg_tbl_t* tbl = malloc(0x5000);
	
	if(!tbl)
		return (pg_tbl_t*)0;
	
	
	uint32_t stepsize = __plat_pg_tbl_maxentry();
	
	void* entry_loc = mem_phys_find_free(stepsize);
	if(!entry_loc)
		return (pg_tbl_t*)0;
	
	mem_phys_set(entry_loc, stepsize);
	
	pg_create(tbl, entry_loc, (size_t)PLATFORM_PROC_MAX_MEM);
	
	uint32_t start_virt = ((uint32_t)virt_start)/stepsize;
	uint32_t end_virt = ((uint32_t)virt_end-1)/stepsize;
	uint32_t fail = 0;
	for(uint32_t i = start_virt; i<=end_virt && !fail; i++)
	{
		void* phys_addr = mem_phys_find_free(stepsize);
		if(phys_addr)
		{
			//printf("mapping %x to %x\r\n", (char*)(i*stepsize), phys_addr);
			pg_map(tbl, (char*)(i*stepsize), phys_addr, stepsize, 0, PERM_PRW_URW, 0, 0, 0);
			mem_phys_set(phys_addr, stepsize);
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
		void* phys_addr = mem_phys_find_free(stepsize);
		if(phys_addr)
		{
			//printf("mapping %x to %x\r\n", (char*)(i*stepsize), phys_addr);
			pg_map(tbl, (char*)(i*stepsize), phys_addr, stepsize, 0, PERM_PRW_URW, 0, 0, 0);
			mem_phys_set(phys_addr, stepsize);
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