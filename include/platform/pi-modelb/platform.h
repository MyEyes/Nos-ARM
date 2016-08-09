#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#include "platform/common.h"

#define GPIO_BASE_ADDR 0x20200000
#define UART0_BASE_ADDR 0x20201000

#define PLATFORM_TOTAL_ADDR_RANGE ((void*)0xFFFFFFFC) //Highest 32 byte aligned address

#define PERIPHERALS_LOW_ADDR 0x20000000		

#define PLATFORM_TOTAL_MEMORY ((void*)0x20000000) //512MB

#define PLATFORM_TTBCR_N 4
#define PLATFORM_PROC_MAX_MEM ((void*)(1<<(32-PLATFORM_TTBCR_N)))//0x10000000 //256MB

#define PLATFORM_CPU_MACHINE_TYPE 0xc42 //See (http://www.arm.linux.org.uk/developer/machines/)
#endif