#ifndef __MMU_H__
#define __MMU_H__

#define MMU_DEBUG
#undef MMU_DEBUG

#include <stdint.h>
#include __PLATFORM__

void mmu_enable();
void mmu_disable();

void mmu_set_kern_pgtbl(void* tbl_addr);
void mmu_set_user_pgtbl(void* tbl_addr);
void mmu_set_user_limit(uint32_t limit);

void domain_manager_set();
void domain_user_set();
uint32_t domain_get_flags();

#endif