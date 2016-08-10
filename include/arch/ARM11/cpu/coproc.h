#ifndef __COPROC_H__
#define __COPROC_H__
#include <stdint.h>

#define VFP_CTRL_1 10
#define VFP_CTRL_2 11
#define DBG_ETM_CTRL 14
#define SYS_CTRL 15

//Workaround to get macro expansion to work properly
#define MCR(CPN, OP1, CRN, CRM, OP2, VAL) _MCR(CPN, OP1, CRN, CRM, OP2, VAL)
#define _MCR(CPN, OP1, CRN, CRM, OP2, VAL) \
		{uint32_t res = VAL;\
		__asm__ __volatile__ ( \
		"mcr p"#CPN", #"#OP1", %0, c"#CRN", c"#CRM", #"#OP2""\
		: : "r"(res) :"cc");}

//Workaround to get macro expansion to work properly		
#define MRC(CPN, OP1, CRN, CRM, OP2, VAL) _MRC(CPN, OP1, CRN, CRM, OP2, VAL)		
#define _MRC(CPN, OP1, CRN, CRM, OP2, VAL) \
		{__asm__ __volatile__ ( \
		"mrc p"#CPN", #"#OP1", %1, c"#CRN", c"#CRM", #"#OP2""\
		: "=r"(VAL) :"0"(VAL) :"cc");}

#endif