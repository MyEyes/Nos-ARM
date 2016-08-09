#define PLATFORM_TOTAL_ADDR_RANGE ((void*)0xFFFFFFFC) //Highest 32 byte aligned address

#define PLATFORM_KERNEL_BASE 0xC0000000
#define PLATFORM_KERNEL_PHYS_SIZE (10<<20) //Reserve 10 Megs for the kernel

#define PLATFORM_KERNEL_VIRT_BASE_OFFSET (PLATFORM_KERNEL_BASE>>20)