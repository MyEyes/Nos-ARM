#ifndef __PAGING_H__
#define __PAGING_H__

#include __PLATFORM__
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#define KERNEL_DEF_PG_LOC PLATFORM_PROC_MAX_MEM
#define KERNEL_DEF_USR_PG_LOC ((char*)KERNEL_DEF_PG_LOC+0x100000)

#define TO_KERNEL_ADDR_SPACE(u) ((((uint32_t)u)<((uint32_t)PLATFORM_KERNEL_BASE))?(((uint32_t)u)+PLATFORM_KERNEL_BASE):((uint32_t)u))

#define SO_CACHE_SIZE 256

typedef struct
{
	pg_fo_desc_t*	addr;
	uint32_t		size;
} pg_tbl_t;

extern pg_tbl_t kernel_page;
extern pg_tbl_t user_page;
extern uint32_t pg_initialized;

void pg_create_default(void* loc, void* usr_loc);

void pg_create(pg_tbl_t* tbl, p_addr_t entry_loc, size_t mem);

void pg_map(pg_tbl_t* tbl, v_addr_t virt_addr, p_addr_t phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared);
void pg_unmap(pg_tbl_t* tbl, v_addr_t virt_addr, size_t mem);
void* pg_get_phys(pg_tbl_t* tbl, v_addr_t virt_addr);
uint32_t pg_get_entry(pg_tbl_t* tbl, v_addr_t virt_addr);

void __plat_pg_map(void* tbl_addr, v_addr_t virt_addr, p_addr_t phys_addr, size_t mem, char domain, char perm, char caching, char global, char shared);
void __plat_pg_unmap(void* tbl_addr, v_addr_t virt_addr, size_t mem);
void* __plat_pg_get_phys(void* tbl_addr, v_addr_t virt_addr);
void* __plat_pg_get_kern(void* tbl_addr, v_addr_t virt_addr);
uint32_t __plat_pg_get_entry(void* tbl_addr, v_addr_t virt_addr);
uint32_t __plat_pg_tbl_maxentry();
void __plat_pg_clear(void* tbl_addr, size_t mem);

#ifdef PLATFORM_HAS_BIG_PAGES //If we can get away with not redirecting to a second level descriptor we need those functions
pg_fo_desc_t __plat_make_first_order_desc(p_addr_t base_addr, char domain, char perm, char caching, char global, char shared);
int __plat_is_big_page(pg_fo_desc_t desc);
#endif

pg_fo_desc_t __plat_make_first_order_pointer(pg_so_desc_t* desc_addr, char domain, char perm, char caching, char global, char shared);
pg_so_desc_t __plat_make_second_order_desc(p_addr_t base_addr, char domain, char perm, char caching, char global, char shared);

void* sys_pmap();

#endif
