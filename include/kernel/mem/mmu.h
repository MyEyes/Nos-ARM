#ifndef __MMU_H__
#define __MMU_H__

#define MMU_DEBUG
#undef MMU_DEBUG

#include <stdint.h>
#include __PLATFORM__
#define TTBR0_ADDR_MSK (((1<<(18+PLATFORM_TTBCR_N))-1)) //17+N 1s since bits 31 to (14-n) are able to be set
#define TTBR0_ADDR_OFFSET (14-PLATFORM_TTBCR_N)
#define TTBR1_ADDR_MSK (((1<<18)-1))
#define TTBR1_ADDR_OFFSET 14

#define MMU_SYSCTL_MAIN 1
#define MMU_SYSCTL_MAIN_ENAB 0

#define MMU_TTB_CTRL 2
	#define MMU_TTB0 0
	#define MMU_TTB1 1
	#define MMU_TTBC 2

void mmu_enable();
void mmu_disable();

void mmu_set_ttbrc(char n);
void mmu_set_ttbr0(void* tbl_addr);
void mmu_set_ttbr1(void* tbl_addr);

void domain_manager_set();

#endif