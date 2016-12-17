#include <string.h>

#include "arch/ARM11/mem/sld.h"
#include "arch/ARM11/mem/fld.h"
#include "kernel/mem/paging.h"
#include "kernel/mem/mem.h"
#include "arch/ARM11/mem/mem.h"

//#define MEM_DBG_TBLS

//#ifdef MEM_DBG_TBLS
#include <stdio.h>
#include "kernel/mod/kernel_uart.h"
//#endif

void* __plat_pg_get_phys(void* fld_tbl, void* virt_addr)
{
	uint32_t u_addr = (uint32_t) virt_addr;
	uint32_t fld_index = u_addr >> 20; //how many MBs
	uint32_t entry = ((uint32_t*)fld_tbl)[fld_index];
	if(FLD_IS_SECTION(entry))
		return (void*)((entry & FLD_SECTION_MASK) | (u_addr&(~FLD_SECTION_MASK)));
	else if(FLD_IS_PGTBL(entry))
	{
		uint32_t* sld_tbl = (uint32_t*)(entry&FLD_PG_TBL_MASK);
		sld_tbl = (uint32_t*)TO_KERNEL_ADDR_SPACE(sld_tbl);
		uint32_t sld_index = u_addr - (fld_index<<20); //Get offset inside 1MB
		sld_index >>= 12;							 //Get 4kb offset
		uint32_t sld_entry = sld_tbl[sld_index];
		//If the page is mapped as a small page
		if(sld_entry&0x2)
			return (void*)((sld_entry & SLD_SMALL_ADDR_MASK) | (u_addr & (~SLD_SMALL_ADDR_MASK)));
	}
	return (void*)0;
}

void* __plat_pg_get_kern(void* fld_tbl, void* virt_addr)
{
	if(!pg_initialized)
		return virt_addr;
		
	if(virt_addr<PLATFORM_PROC_MAX_MEM)
		return (void*)TO_KERNEL_ADDR_SPACE(__plat_pg_get_phys(fld_tbl, virt_addr));
	else if((uint32_t)virt_addr<PLATFORM_KERNEL_BASE)
		return (void*)TO_KERNEL_ADDR_SPACE(virt_addr);
	else
		return virt_addr;
}

void __plat_pg_clear(void* tbl_addr, size_t mem)
{
	uint32_t roundup = mem&((1<<20)-1);
	uint32_t num = mem>>20;
	if(roundup)
		num+=1;
	memset((char*)tbl_addr,0, num * 4);
}

uint32_t __plat_pg_get_entry(void* fld_tbl, void* virt_addr)
{
	uint32_t u_addr = (uint32_t) virt_addr;
	uint32_t fld_index = u_addr >> 20; //how many MBs
	uint32_t entry = ((uint32_t*)fld_tbl)[fld_index];
	if(FLD_IS_SECTION(entry))
		return entry;
	else if(FLD_IS_PGTBL(entry))
	{
		uint32_t* sld_tbl = (uint32_t*)(entry&FLD_PG_TBL_MASK);
		uint32_t sld_index = u_addr - (fld_index<<20); //Get offset inside 1MB
		sld_index >>= 12;							 //Get 4kb offset
		uint32_t sld_entry = sld_tbl[sld_index];
		//If the page is mapped as a small page
		if(sld_entry&0x2)
			return sld_entry;
	}
	return 0;
}

uint32_t __plat_pg_tbl_maxentry()
{
	return 1<<20;
}

pg_fld_t __plat_fld_big(p_addr_t base_addr, char domain, char perm, char caching, char global, char shared)
{
	return (pg_fld_t) fld_construct_section(base_addr, domain, perm, caching, global, shared);
}

int __plat_is_big_page(pg_fld_t desc)
{
	return FLD_IS_SECTION(desc);
}

int __plat_get_fld_parms(pg_fld_t desc, p_addr_t* addr, char* domain, char* perm, char* caching, char* global, char* shared)
{
	if(!desc)
		return -1;
	*addr = (p_addr_t)(desc & FLD_SECTION_MASK);
	*domain = (desc >> FLD_ENTRY_DOMAIN_OFFSET) & FLD_ENTRY_DOMAIN_MASK;
	//fld |= (domain<<5);
		
	char nx = (desc >> FLD_ENTRY_XN_OFFSET) & 1;
	char ap = (desc >> FLD_ENTRY_AP_OFFSET) & 3;
	char apx = (desc >> FLD_ENTRY_APX_OFFSET) & 1;
	
	*perm = (nx<<3) | (apx<<2) | ap;
	
	char tex = (desc >> FLD_ENTRY_TEX_OFFSET) & 7;
	char c = (desc >> FLD_ENTRY_C_OFFSET) & 1;
	char b = (desc >> FLD_ENTRY_B_OFFSET) & 1;
	
	*caching = tex<<2 | c<<1 | b;
	
	*global = !((desc>>FLD_ENTRY_NG_OFFSET) & 1);
	*shared = (desc >> FLD_ENTRY_S_OFFSET) & 1;
	return 0;
}

pg_fld_t __plat_fld_create(pg_sld_t* desc_addr, char domain, char perm, char caching, char global, char shared)
{
	(void)domain;
	(void)perm;
	(void)caching;
	(void)global;
	(void)shared;
	return fld_construct_sld((p_addr_t)desc_addr, domain);
}

pg_sld_t* __plat_sld_of_fld(pg_fld_t fld)
{
	return (pg_sld_t*)(fld&FLD_PG_TBL_MASK);
}

pg_sld_t __plat_sld_create(p_addr_t base_addr, char domain, char perm, char caching, char global, char shared)
{
	(void)domain;
	return sld_create_small_entry(base_addr, perm, caching, global, shared);
}

p_addr_t __plat_get_sld_addr(pg_sld_t sld)
{
	return (p_addr_t)(sld & SLD_SMALL_ADDR_MASK);
}
