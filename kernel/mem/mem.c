#include <stdint.h>
#include "kernel/mem/mem.h"
#include "kernel/mem/paging.h"
#include "std/string.h"
#define MEM_DBG_TBLS
#ifdef MEM_DBG_TBLS
#include "std/stdio.h"
#endif

MB_phys_desc_t* desc_tbl = 0;
size_t num_descs = 0;

uint32_t mem_phys_free = (uint32_t)PLATFORM_TOTAL_MEMORY;
uint32_t mem_phys_used = 0;

void mem_phys_init(size_t total_mem)
{
	desc_tbl = KERNEL_PHYS_MEM_TBL;
	num_descs = total_mem >> 20; //number of entries, if there is a partial MB somehow, we just throw it away
}

void mem_phys_reset()
{
	memset((char*)desc_tbl, 0, num_descs*sizeof(desc_tbl));
	mem_plat_init();
}

void mem_phys_set(void* phys_addr, size_t mem)
{
	mem_phys_set_val(phys_addr, mem, 1);
	mem_phys_free -= mem;
	mem_phys_used += mem;
}

void mem_phys_clear(void* phys_addr, size_t mem)
{
	mem_phys_set_val(phys_addr, mem, 0);
	mem_phys_used -= mem;
	mem_phys_free += mem;
}


void mem_phys_set_val(void* phys_addr, size_t mem, char val)
{
	uint32_t phys_addr_u = (uint32_t) phys_addr;
	uint32_t phys_addr_last = ((uint32_t) phys_addr) + mem;
	uint32_t index = (phys_addr_u>>20);
	uint32_t endindex = (phys_addr_last>>20);
	
	for(; index<=endindex; index++)
	{
		MB_phys_desc_t* curr_desc = desc_tbl + index;
		//figure out how much we still have to set
		uint32_t curr_len = phys_addr_last - phys_addr_u;

		uint32_t curr_offset = phys_addr_u-(index<<20);
		//if it's more than 1MB, round down to 1MB
		if(curr_len > (1<<20))
		{
			curr_len = 1<<20;
			curr_len -= curr_offset;
		}
		if(val)
			mem_phys_desc_set(curr_desc, curr_offset, curr_len);
		else
			mem_phys_desc_clear(curr_desc, curr_offset, curr_len);
		
		phys_addr_u = (index+1)<<20;
	}
}

void* mem_phys_find_free(size_t mem)
{
	//If we are asking for over 1MB return the null pointer
	//the point is that we will only return one address and finding several MB of contiguous memory seems unlikely
	if(mem > (1<<20))
		return 0;
	else if(mem==0)
		return 0;
	else
	{
		uint32_t curr_len = 256; //Number of 4kb entries
		uint32_t curr_offset = 256; //Offset of first 4kb entry from struct start in bits (32*8)
		uint32_t curr_size = 0x1000; //4kb
		mem -= 1;
		mem >>= 12; //Round mem down to 4kb boundary
		//Since we don't have a log function we can just loop this
		//to figure out how much we need to allocate and what resolution to look at.
		while(mem>0)
		{
			mem >>= 1;
			curr_len >>= 1;
			curr_offset >>= 1;
			curr_size <<= 1;
		}
		
		
		
		for(uint32_t curr_desc = 0; curr_desc<num_descs; curr_desc++)
		{
			for(uint32_t curr_entry = 0; curr_entry<curr_len; curr_entry++)
				if(!mem_bit_set((char*)(desc_tbl+curr_desc), curr_offset+curr_entry))
					return (void*)((curr_desc<<20)+curr_size*curr_entry);
		}
		//If we can't find an entry that fits, we do this
		return 0;
	}
}

void mem_phys_desc_set(MB_phys_desc_t* desc, uint32_t offset, size_t mem)
{
	if(mem==0)
		return;
	
	uint32_t curr_offset = offset>>12; //(shift right to get alignment to 4kb)
	uint32_t end_offset = (offset+mem-1)>>12;
		
	char* desc_c = (char*)desc;
	
	//Step is the offset from the bitmaps start in bits for each resolution
	//So we potentially touch any bit but the first
	for(uint32_t step = 32*8; step>=1; step>>=1)
	{
		char* curr = desc_c;
		mem_set_bits(curr, curr_offset+step, end_offset+step);
		curr_offset >>= 1;
		end_offset >>= 1;
	}
	
}

void mem_phys_desc_clear(MB_phys_desc_t* desc, uint32_t offset, size_t mem)
{
	if(mem==0)
		return;
		
	int32_t curr_offset = offset>>12; //(shift right to get alignment to 4kb)
	int32_t end_offset = (offset+mem-1)>>12;
		
	char* desc_c = (char*)desc;
	//Step is the offset from the bitmaps start in bits for each resolution
	//So we potentially touch any bit but the first
	for(uint32_t step = 32*8; step>=1; step>>=1)
	{
		char* curr = desc_c;
		mem_clear_bits(curr, curr_offset + step, end_offset + step);
		//If we are at an odd position and the previous bit is not clear, we can not set the top level of that as clear
		if(curr_offset&0x1)
			if(mem_bit_set(curr, curr_offset + step - 1))
				curr_offset += 1;
		//If we end at an even position and the following bit is not clear, we can not set the top level of that as clear
		if(!(end_offset&0x1))
			if(mem_bit_set(curr, end_offset + step + 1))
				end_offset -= 1;
		//The only way this can happen is if they have the same value but we can't set the level above it
		//So if this occurs, we stop clearing bits
		if(curr_offset>end_offset)
			break;
		curr_offset >>= 1;
		end_offset >>= 1;
	}
}

void mem_mark_as_kernel(void* phys_addr, size_t mem)
{
	uint32_t index = ((uint32_t)phys_addr)>>20;
	uint32_t end_index = ((uint32_t)phys_addr+mem)>>20;
	for(;index<=end_index; index++)
		desc_tbl[index].large_chunks |= 1;
}

void mem_unmark_as_kernel(void* phys_addr, size_t mem)
{
	uint32_t index = ((uint32_t)phys_addr)>>20;
	uint32_t end_index = ((uint32_t)phys_addr+mem)>>20;
	for(;index<=end_index; index++)
		desc_tbl[index].large_chunks &= 0xFE;
}

void mem_set_bits(char* base, uint32_t start_bit, uint32_t end_bit)
{
	char s_bit_under = start_bit & 0x7; 
	char e_bit_over = ((end_bit+1) & 0x7); 
	
	uint32_t start = (start_bit + 7)>>3; 
	uint32_t end = (end_bit+1)>>3; 
	
	//If bits are not byte aligned for the start
	if((start_bit>>3)==end)
		base[start_bit>>3] |= ((1 << (e_bit_over - s_bit_under))-1) << s_bit_under;
	else
	{
		if(s_bit_under)
			base[start_bit>>3] |= ((unsigned char)-1) << s_bit_under;
		if(e_bit_over)
			base[end] |= (1<<e_bit_over)-1;
	}
	
	for(uint32_t c = start; c<end; c++)
		base[c] = (unsigned char)-1;
}

void mem_clear_bits(char* base, uint32_t start_bit, uint32_t end_bit)
{
	char s_bit_under = start_bit & 0x7; 
	char e_bit_over = ((end_bit+1) & 0x7);
	
	uint32_t start = (start_bit + 7)>>3;
	uint32_t end = (end_bit+1)>>3;
		
	//If bits are not byte aligned for the start
	if((start_bit>>3)==end)
		base[start_bit>>3] &= ~(((1 << (e_bit_over - s_bit_under))-1) << s_bit_under);
	else
	{
		if(s_bit_under)
			base[start_bit>>3] &= ~(((unsigned char)-1) << s_bit_under);
		if(e_bit_over)
			base[end] &= ~((1<<e_bit_over)-1);
	}
	
	for(uint32_t c = start; c<end; c++)
		base[c] = (unsigned char)0;
}

char mem_bit_set(char* base, uint32_t bit)
{
	uint32_t byte = bit >> 3;
	bit &= 7;
	return (base[byte]>>bit) & 1;
}
