#include "arch/ARM11/mem/sld.h"
#include "kernel/mem/perm.h"
#include "arch/ARM11/mem/cache.h"
#include <stdio.h>


uint32_t sld_create_small_entry(p_addr_t phys_addr, char perm, char caching, char global, char shared)
{
	uint32_t u_addr = (uint32_t) phys_addr;
	u_addr &= SLD_SMALL_ADDR_MASK;
	//Set permissions
	u_addr |= PERM_NX(perm)?1:0; //Set NX bit if in perm
	u_addr |= PERM_AP(perm)<<SLD_AP_OFFSET;
	u_addr |= PERM_APX(perm)<<SLD_APX_OFFSET;
	//Set caching
	u_addr |= CACHE_TEX(caching)<<SLD_STEX_OFFSET;
	u_addr |= CACHE_C(caching)<<SLD_C_OFFSET;
	u_addr |= CACHE_B(caching)<<SLD_B_OFFSET;
	//Set global and shared attributes
	u_addr |= global?0:1<<SLD_NG_OFFSET; //Since the flag is actually NG(not global)
	u_addr |= shared?1<<SLD_S_OFFSET:0;
	return u_addr | SLD_TYPE_SMALL_PAGE;
}
