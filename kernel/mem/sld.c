#include "kernel/mem/sld.h"
#include "kernel/mem/perm.h"



uint32_t sld_create_small_entry(void* phys_addr, char perm, char caching, char global, char shared)
{
	uint32_t u_addr = (uint32_t) phys_addr;
	u_addr &= SLD_SMALL_ADDR_MASK;
	u_addr |= PERM_NX(perm)?1:0; //Set NX bit if in perm
	u_addr |= PERM_AP(perm)<<SLD_AP_OFFSET;
	u_addr |= PERM_APX(perm)<<SLD_APX_OFFSET;
	//TODO: set caching behaviour
	#warning "caching behaviour for slds is not being set"
	u_addr |= global?0:1<<SLD_NG_OFFSET; //Since the flag is actually NG(not global)
	u_addr |= shared?1<<SLD_S_OFFSET:0;
	return u_addr | SLD_TYPE_SMALL_PAGE;
}