#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#include "platform/pi-modelb/int.h"
#include "platform/pi-modelb/clock.h"
#include "platform/pi-modelb/bus.h"
#include "platform/arm-common.h"

#define GPIO_BASE_ADDR 0x20200000
#define UART0_BASE_ADDR 0x20201000

#define PERIPHERALS_LOW_ADDR 0x20000000		

#define PLATFORM_TOTAL_MEMORY ((void*)0x20000000) //512MB

#define PLATFORM_TTBCR_N 4
#define PLATFORM_PROC_MAX_MEM ((void*)(1<<(32-PLATFORM_TTBCR_N)))//0x10000000 //256MB

#define PLATFORM_KERNEL_VIRT_BASE_OFFSET (PLATFORM_KERNEL_BASE>>20)
#define PLATFORM_KERNEL_VIRT_SIZE (PLATFORM_KERNEL_PHYS_SIZE>>20)

#define PLATFORM_CPU_MACHINE_TYPE 0xc42 //See (http://www.arm.linux.org.uk/developer/machines/)
#endif