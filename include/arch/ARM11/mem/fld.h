#ifndef __FLD_H__
#define __FLD_H__

#include <stdint.h>
#include __PLATFORM__

//First level descriptor
#define FLD0_ADDR_MASK (((1<<(18+PLATFORM_TTBCR_N))-1)<<(14-PLATFORM_TTBCR_N))
#define FLD1_ADDR_MASK (((1<<(18))-1)<<(14))

#define FLD_ENTRY_TRANS_FAULT 0x00
#define FLD_ENTRY_PG_TBL_BASE_ADDR 0x01
#define FLD_ENTRY_SECTION_BASE_ADDR 0x02
#define FLD_ENTRY_RESERVED 0x03

#define FLD_ENTRY_B_OFFSET 2				//Bufferable	Memory region attribute bits
#define FLD_ENTRY_C_OFFSET 3				//Cacheable
#define FLD_ENTRY_XN_OFFSET 4				//Execute Never Bit
#define FLD_ENTRY_DOMAIN_MASK ((1<<4)-1)	
#define FLD_ENTRY_DOMAIN_OFFSET 5			//Domain Field
#define FLD_ENTRY_P_OFFSET 9				//(Privileged?)
#define FLD_ENTRY_AP_OFFSET 10				//Access Permissions (00:No_Access, 01:No_Access_Usr, 10:R, 11:RW)
#define FLD_ENTRY_TEX_OFFSET 12				//Type Extension Field (Describes caching behaviour together with B and C)
#define FLD_ENTRY_APX_OFFSET 15				//1:RW, 0:R
#define FLD_ENTRY_S_OFFSET 16				//shared bit
#define FLD_ENTRY_NG_OFFSET 17				//Not-global bit
#define FLD_ENTRY_NS_OFFSET 19				//Not secure bit


#define FLD_SECTION_BASE_ADDR_MASK ((1<<12)-1)
#define FLD_SECTION_BASE_ADDR_OFFSET 20 //Section base addr, used if [1:0] is 2
#define FLD_SECTION_MASK (FLD_SECTION_BASE_ADDR_MASK << FLD_SECTION_BASE_ADDR_OFFSET)

#define FLD_PG_TBL_BASE_ADDR_MASK ((1<<22)-1)
#define FLD_PG_TBL_BASE_ADDR_OFFSET 10
#define FLD_PG_TBL_MASK (FLD_PG_TBL_BASE_ADDR_MASK<<FLD_PG_TBL_BASE_ADDR_OFFSET)

#define FLD_IS_SECTION(u) ((u&0x3)==FLD_ENTRY_SECTION_BASE_ADDR)
#define FLD_IS_PGTBL(u) ((u&0x3)==FLD_ENTRY_PG_TBL_BASE_ADDR)


uint32_t* create_flat_fld0(uint32_t* location, uint32_t* start_phys_addr, uint32_t* end_phys_addr);
uint32_t* create_flat_fld1(uint32_t* location, uint32_t* start_phys_addr, uint32_t* end_phys_addr);

void fld_set(uint32_t* fld_addr, uint32_t value);
void fld_clear(void* fld_addr);
uint32_t fld_construct_section(void* base_addr, char domain, char perm, char caching, char global, char shared);
uint32_t fld_construct_sld(void* base_addr, char domain);



#endif