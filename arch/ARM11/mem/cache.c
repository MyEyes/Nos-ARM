#include "arch/ARM11/mem/cache.h"
#include "kernel/mem/cache.h"

void cache_TLB_flush()
{
	MCR(15, 0,8, 7, 0, 0);
}

void data_cache_clean_va(v_addr_t v_addr)
{
    MCR(15, 0, 7, 10, 1, v_addr);
}

void data_cache_clean()
{
    MCR(15, 0, 7, 10, 0, 0);
}
