#ifndef __CACHE_H__
#define __CACHE_H__

#include <sys/types.h>

void cache_TLB_flush();
void data_cache_clean();
void data_cache_clean_va(v_addr_t v_addr);

#endif
