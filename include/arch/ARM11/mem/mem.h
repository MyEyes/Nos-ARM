#ifndef __A_MEM_H__
#define __A_MEM_H__
#include __PLATFORM__

#include <stddef.h>
#include <stdint.h>

#define MEM_SLD_CACHE_SIZE 32

void mem_create_sld(uint32_t* fld_entry_addr, uint32_t offset, uint32_t len, void* phys_base, char domain, char perm, char caching, char global, char shared);
uint32_t* get_free_sld();
void free_sld(uint32_t*);

#endif