#ifndef __MEM_H__
#define __MEM_H__
#include __PLATFORM__

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include "kernel/mem/paging.h"

#define KERNEL_PHYS_MEM_TBL (void*)(PLATFORM_KERNEL_BASE+0x500000) //5MB after kernel image
#define KERNEL_PHYS_MEM_TBL_SIZE (PLATFORM_TOTAL_MEMORY>>14) //64 byte per megabyte (20-6)

#define MEM_DBG_TBLS
#undef MEM_DBG_TBLS

//Describes allocations of physical memory down to 4kb(pagesize) resolution
typedef struct
{
	char large_chunks; //[3:0] 4x256byte bit flags, [5:4] 2x512byte bit flags, [6] 1x1MB bit flag, [7] kernel_memory bit flag
	char byte_128_chunk;
	char byte_64_chunk[2];
	char byte_32_chunk[4];
	char byte_16_chunk[8];
	char byte_8_chunk[16];
	char byte_4_chunk[32];
} MB_phys_desc_t;

typedef struct
{
	uint32_t* 	location;
	uint32_t	size;
} fld_desc_t;

//start managing physical memory
void mem_phys_init(size_t total_mem);
void mem_phys_reset();
void mem_phys_set(p_addr_t phys_addr, size_t mem);
void mem_phys_clear(p_addr_t phys_addr, size_t mem);
void mem_phys_set_val(p_addr_t phys_addr, size_t mem, char val);

void* mem_phys_find_free(size_t mem);

void mem_phys_desc_set(MB_phys_desc_t* desc, uint32_t offset, size_t mem);
void mem_phys_desc_clear(MB_phys_desc_t* desc, uint32_t offset, size_t mem);

void mem_mark_as_kernel(p_addr_t phys_addr, size_t mem);
void mem_unmark_as_kernel(p_addr_t phys_addr, size_t mem);

void mem_set_bits(char* base, uint32_t start_bit, uint32_t end_bit);
void mem_clear_bits(char* base, uint32_t start_bit, uint32_t end_bit);
char mem_bit_set(char* base, uint32_t bit);

void mem_plat_init(); //define by architecture

void mem_dsb();
void mem_dmb();

#endif