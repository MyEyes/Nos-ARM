#include "arch/ARM11/mem/fld.h"
#include "kernel/mem/perm.h"
#include "arch/ARM11/mem/cache.h"

void fld_set(v_addr_t fld_addr, uint32_t value)
{
	
	*((uint32_t*)fld_addr) = value;
}

void fld_clear(v_addr_t fld_addr)
{
	fld_set(fld_addr, 0);
}

uint32_t fld_construct_section(p_addr_t base_addr, char domain, char perm, char caching, char global, char shared)
{
	uint32_t fld = ((uint32_t)base_addr) & (FLD_SECTION_MASK);
	fld |= (domain<<5);
	fld |= FLD_ENTRY_SECTION_BASE_ADDR;
	//Set permissions
	fld |= PERM_NX(perm)<<FLD_ENTRY_XN_OFFSET; //Set NX bit if in perm
	fld |= PERM_AP(perm)<<FLD_ENTRY_AP_OFFSET;
	fld |= PERM_APX(perm)<<FLD_ENTRY_APX_OFFSET;
	//Set caching
	fld |= CACHE_TEX(caching)<<FLD_ENTRY_TEX_OFFSET;
	fld |= CACHE_C(caching)<<FLD_ENTRY_C_OFFSET;
	fld |= CACHE_B(caching)<<FLD_ENTRY_B_OFFSET;
	//Set global and shared flags
	fld |= global?0:1<<FLD_ENTRY_NG_OFFSET; //Since the flag is actually NG(not global)
	fld |= shared?1<<FLD_ENTRY_S_OFFSET:0;
	return fld;
}

uint32_t fld_construct_sld(p_addr_t base_addr, char domain)
{
	return ((((uint32_t)base_addr)&(FLD_PG_TBL_BASE_ADDR_MASK<<FLD_PG_TBL_BASE_ADDR_OFFSET)) | (domain<<5) | FLD_ENTRY_PG_TBL_BASE_ADDR);
}

//Identity map a region of physical memory
uint32_t* create_flat_fld0(v_addr_t location, uint32_t* start_phys_addr, uint32_t* end_phys_addr)
{
	//Mask start location to be aligned
	uint32_t* startLoc = (uint32_t*)(((uint32_t)location)&FLD0_ADDR_MASK);
		
	uint32_t count = ((uint32_t)end_phys_addr-(uint32_t)start_phys_addr)>>20; //difference/1MB
	
	uint32_t i = 0;
	for(i=0; i<count; i++)
		//Make sure we map the kernel pages to where they should be
		if(i<PLATFORM_KERNEL_VIRT_BASE_OFFSET || i>=PLATFORM_KERNEL_VIRT_BASE_OFFSET+PLATFORM_KERNEL_VIRT_SIZE)
			fld_set(startLoc+i, fld_construct_section(start_phys_addr + (i<<(20-2)), 0, PERM_PRW_URW, 0, 1, 0)); //i*1MB
		else
			fld_set(startLoc+i, fld_construct_section(start_phys_addr + ((i-PLATFORM_KERNEL_VIRT_BASE_OFFSET)<<(20-2)), 0, PERM_PRW_URW, 0, 1, 0));
	return startLoc;
}

//Identity map a region of physical memory
uint32_t* create_flat_fld1(v_addr_t location, uint32_t* start_phys_addr, uint32_t* end_phys_addr)
{
	//Mask start location to be aligned
	uint32_t* startLoc = (uint32_t*)(((uint32_t)location)&FLD1_ADDR_MASK);
		
	uint32_t count = ((uint32_t)(end_phys_addr-start_phys_addr))>>(20-2); //difference/1MB, but since they are 4byte pointers we only divide by 256kb
	
	uint32_t i = 0;
	for(i=0; i<count; i++)
		//Make sure we map the kernel pages to where they should be
		if(i<PLATFORM_KERNEL_VIRT_BASE_OFFSET || i>=PLATFORM_KERNEL_VIRT_BASE_OFFSET+(PLATFORM_KERNEL_PHYS_SIZE>>20))
			fld_set(startLoc+i, fld_construct_section(start_phys_addr + (i<<(20-2)), 0, PERM_PRW_URW, 0, 1, 0)); //i*1MB
		else
			fld_set(startLoc+i, fld_construct_section(start_phys_addr + ((i-PLATFORM_KERNEL_VIRT_BASE_OFFSET)<<(20-2)), 0, PERM_PRW_URW, 0, 1, 0));
	return startLoc;
}