#include "arch/ARM11/mem/cache.h"
#include "kernel/mem/cache.h"

void cache_TLB_flush()
{
	MCR(15, 0,8, 7, 0, 0);
}