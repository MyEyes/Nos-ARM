#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>

#define MEM_ALIGNMENT 4 //must be a power of 2
#define MEM_ALIGN_BY_4(x) (((x) + (MEM_ALIGNMENT-1)) & ~(MEM_ALIGNMENT-1)) //round up to multiple of 4
#define MEM_HEADER_SIZE (MEM_ALIGN_BY_4(sizeof(struct mem_area_tt)))

typedef struct mem_area_tt
{
	struct mem_area_tt* next;
	struct mem_area_tt* prev;
	size_t 				size;
	uint32_t 			free; //0 is false, 1 is true
} mem_area_t;

void mem_init();
mem_area_t* mem_find_free_area(size_t size);
mem_area_t* mem_split_area(mem_area_t* area, size_t size);
void mem_join_area(mem_area_t* area);
mem_area_t* mem_extend_heap(size_t size);

void mem_extend(size_t size, int free);

#endif