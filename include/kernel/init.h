#ifndef __INIT_H__
#define __INIT_H__

#include __PLATFORM__

#define KERNEL_FLD0_LOC PLATFORM_PROC_MAX_MEM
#define KERNEL_FLD1_LOC (PLATFORM_PROC_MAX_MEM+0x10000)

void kernel_init();
#endif