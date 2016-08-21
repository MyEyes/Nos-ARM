#ifndef __A_CACHE_H__
#define __A_CACHE_H__

#include "arch/ARM11/cpu/coproc.h"

#define CACHE_TYPE_STRONGORDER 	0b00000 //strongly ordered memory operations
#define CACHE_TYPE_SHAREDDEV	0b00001 //shared device
#define CACHE_TYPE_OI_WT_NW		0b00010 //outer and inner write through, no write allocate
#define CACHE_TYPE_OI_WB_NW		0b00011 //outer and inner write back, no write allocate
#define CACHE_TYPE_OI_NC		0b00100	//outer and inner non cacheable
#define CACHE_TYPE_OI_WB_W		0b00111 //outer and inner write back, write allocate
#define CACHE_TYPE_NSHAREDDEV	0b01000	//non shared device
#define CACHE_TYPE_I_O			0b10000 //last 4 bits define inner and outer cacheability, <<2 cache_pol for outer policy <<0 cache_pol for inner policy

#define CACHE_POL_NC			0b00	//no cache
#define CACHE_POL_WBWA			0b01	//write back, write allocate
#define CACHE_POL_WTNW			0b10	//write through, no write allocate
#define CACHE_POL_WBNW			0b11	//write back, no write allocate

#define CACHE_TEX(u)			(u>>2)		//returns the TEX portion of the cache value
#define CACHE_C(u)				((u>>1)&1)	//returns the C bit of the cache value
#define CACHE_B(u)				(u&1)		//returns the B bit of the cache value

#endif