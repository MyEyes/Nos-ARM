#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#include "platform/qemu-pi/clock.h"
#include "platform/qemu-pi/bus.h"
#include "platform/qemu-pi/int.h"
#include "arch/ARM11/arch.h"

#define PERIPHERALS_LOW_ADDR 0x3F000000		

#define PLATFORM_TOTAL_MEMORY ((void*)0x20000000) //512MB

#define PLATFORM_TTBCR_N 5
#define PLATFORM_PROC_MAX_MEM ((void*)(1<<(32-PLATFORM_TTBCR_N)))//0x8000000 //128MB

#define PLATFORM_CPU_MACHINE_TYPE 0xc42

#define GPIO_BASE_ADDR 0x3F200000
#define UART0_BASE_ADDR 0x3F201000

#define PLATFORM_KERNEL_VIRT_BASE_OFFSET (PLATFORM_KERNEL_BASE>>20)
#define PLATFORM_KERNEL_VIRT_SIZE (PLATFORM_KERNEL_PHYS_SIZE>>20)

#endif
