#ifndef __PROC_H__
#define __PROC_H__

#include <stdint.h>
#include "kernel/mem/paging.h"
#include __PLATFORM__

typedef struct
{
	uint32_t pid;
	pg_tbl_t* pg_tbl;
} proc_hdr_t;

extern proc_hdr_t kern_proc;

void proc_init(proc_hdr_t* proc, pg_tbl_t* tbl, uint32_t pid);

#endif