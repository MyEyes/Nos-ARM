#ifndef __PAGING_H__
#define __PAGING_H__

#include __PLATFORM__
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#define KERNEL_DEF_PG_LOC PLATFORM_PROC_MAX_MEM
#define KERNEL_DEF_USR_PG_LOC ((char*)KERNEL_DEF_PG_LOC+0x100000)

#define TO_KERNEL_ADDR_SPACE(u) ((((uint32_t)u)<((uint32_t)PLATFORM_KERNEL_BASE))?(((uint32_t)u)+PLATFORM_KERNEL_BASE):((uint32_t)u))

#define SLD_CACHE_SIZE 256

//Check that PAGE_SIZE is a power of two
#if !PAGE_SIZE || (PAGE_SIZE & (PAGE_SIZE - 1)) != 0
	#error PAGE_SIZE is not a power of 2
#endif
//Check that BIG_PAGE_SIZE is a power of two
#if !BIG_PAGE_SIZE || (BIG_PAGE_SIZE & (BIG_PAGE_SIZE - 1)) != 0
	#error BIG_PAGE_SIZE is not a power of 2
#endif


typedef struct
{
	pg_fld_t*	addr;
	uint32_t	size;
} pg_tbl_t;

extern pg_tbl_t kernel_page;
extern pg_tbl_t user_page;
extern uint32_t pg_initialized;

void pg_create_default(void* loc, void* usr_loc);
void pg_create(pg_tbl_t* tbl, p_addr_t entry_loc, size_t mem);

int pg_map(pg_tbl_t* tbl, v_addr_t virt_addr, p_addr_t phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared);
int pg_map_secondary(pg_fld_t* fld, v_addr_t virt_addr, p_addr_t phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared);
int pg_unmap(pg_tbl_t* tbl, v_addr_t virt_addr, size_t mem);
void* pg_get_phys(pg_tbl_t* tbl, v_addr_t virt_addr);
uint32_t pg_get_entry(pg_tbl_t* tbl, v_addr_t virt_addr);

void pg_free(pg_fld_t* fld);
void pg_free_sld(pg_sld_t* sld);
pg_sld_t* pg_get_sld();

//void __plat_pg_map(void* tbl_addr, v_addr_t virt_addr, p_addr_t phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared);
//void __plat_pg_unmap(void* tbl_addr, v_addr_t virt_addr, size_t mem);
void* __plat_pg_get_phys(void* tbl_addr, v_addr_t virt_addr);
void* __plat_pg_get_kern(void* tbl_addr, v_addr_t virt_addr);
uint32_t __plat_pg_get_entry(void* tbl_addr, v_addr_t virt_addr);
uint32_t __plat_pg_tbl_maxentry();
void __plat_pg_clear(void* tbl_addr, size_t mem);

#ifdef PLATFORM_HAS_BIG_PAGES //If we can get away with not redirecting to a second level descriptor we need those functions
pg_fld_t __plat_fld_big(p_addr_t base_addr, char domain, char perm, char caching, char global, char shared);
int __plat_is_big_page(pg_fld_t desc);
int __plat_get_fld_parms(pg_fld_t desc, p_addr_t* addr, char* domain, char* perm, char* caching, char* global, char* shared);
#endif

pg_fld_t __plat_fld_create(pg_sld_t* desc_addr, char domain, char perm, char caching, char global, char shared);
pg_sld_t* __plat_sld_of_fld(pg_fld_t fld);
pg_sld_t __plat_sld_create(p_addr_t base_addr, char domain, char perm, char caching, char global, char shared);
p_addr_t __plat_get_sld_addr(pg_sld_t sld);

void* sys_pmap();

#endif
