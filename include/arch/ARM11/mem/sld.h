#ifndef __SLD_H__
#define __SLD_H__

#include <stdint.h>
#include <sys/types.h>

#define SLD_TYPE_LARGE_PAGE 1
#define SLD_TYPE_SMALL_PAGE 2

#define SLD_NX_OFFSET 0

#define SLD_B_OFFSET 2
#define SLD_C_OFFSET 3
#define SLD_AP0_OFFSET 4
#define SLD_AP1_OFFSET 6
#define SLD_AP2_OFFSET 8
#define SLD_AP3_OFFSET 10
#define SLD_TEX_OFFSET 12

#define SLD_AP_OFFSET 4
#define SLD_STEX_OFFSET 6
#define SLD_APX_OFFSET 9
#define SLD_S_OFFSET 10
#define SLD_NG_OFFSET 11

#define SLD_LARGE_PAGE_ADDR_OFFSET 16
#define SLD_SMALL_PAGE_ADDR_OFFSET 12

#define SLD_SMALL_ADDR_MASK (((1<<20)-1)<<12)

uint32_t sld_create_small_entry(p_addr_t phys_addr, char perm, char caching, char global, char shared);
	
#endif
