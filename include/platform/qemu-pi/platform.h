#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#include "platform/common.h"

#define PERIPHERALS_LOW_ADDR 0x20000000		

#define PLATFORM_TOTAL_MEMORY ((void*)0x10000000) //256MB

#define PLATFORM_TTBCR_N 5
#define PLATFORM_PROC_MAX_MEM ((void*)(1<<(32-PLATFORM_TTBCR_N)))//0x8000000 //128MB

#define PLATFORM_CPU_MACHINE_TYPE 4828

#define GPIO_BASE_ADDR 0x20200000
#define UART0_BASE_ADDR 0x20201000
#endif