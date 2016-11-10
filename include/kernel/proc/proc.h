#ifndef __PROC_H__
#define __PROC_H__

#include <stdint.h>
#include "kernel/mem/paging.h"
#include __PLATFORM__

//Processes will have a reserved memory area between 0x80000 and 0x100000
//for mapping kernel ressources (i.e. ipc_ports, mmapped physical regions etc)

#define PROC_KRES_LOC	0x80000
#define PROC_KRES_SIZE	0x80000
#define PROC_KRES_END   (PROC_KRES_LOC+PROC_KRES_SIZE)

typedef struct
{
	uint32_t 	pid;		//Process ID
	uint32_t 	priv;		//Privilege level
	pg_tbl_t* 	pg_tbl;		//Page table
	uint32_t 	vs;			//Virtual address start
	uint32_t 	brk;		//Program break
} proc_hdr_t;

extern proc_hdr_t kern_proc;

void proc_init(proc_hdr_t* proc, pg_tbl_t* tbl, uint32_t virt_start, uint32_t virt_end, uint32_t priv);
pg_tbl_t* proc_create(char* virt_start, char* virt_end, uint32_t stack_size);

void* sbrk();
uint32_t fork();

#endif
