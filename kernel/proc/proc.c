#include "kernel/proc/proc.h"

proc_hdr_t kern_proc;

void proc_init(proc_hdr_t* proc, pg_tbl_t* tbl, uint32_t pid)
{
	proc->pid=pid;
	proc->pg_tbl=tbl;
}