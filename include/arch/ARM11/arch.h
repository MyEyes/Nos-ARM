#include <stdint.h>

#define PLATFORM_TOTAL_ADDR_RANGE ((void*)-1) //No limit on address range

#define PLATFORM_KERNEL_BASE (0xC0000000)
#define PLATFORM_KERNEL_PHYS_SIZE (10<<20) //Reserve 10 Megs for the kernel

#define PLATFORM_REG_STATE_SIZE (13*4+1*4) //13 general purpose registers + 1 program state register //pc and sp are stored separately

#define PLATFORM_SYSCALL(id, r) _PLATFORM_SYSCALL(id, r)
	#define _PLATFORM_SYSCALL(id, r) __asm__ __volatile__("swi "#id"\n bx lr\nmov %0, r0":"=r"(r))
#define PLATFORM_SYSCALL1(id, p1, r) _PLATFORM_SYSCALL1(id, p1, r)
	#define _PLATFORM_SYSCALL1(id, p1, r) __asm__ __volatile__("mov r0, %1\nswi "#id"\n bx lr\nmov %0, r0":"=r"(r):"r"(p1))
#define PLATFORM_SYSCALL2(id, p1, p2, r) _PLATFORM_SYSCALL2(id, p1, p2, r)
	#define _PLATFORM_SYSCALL2(id, p1, p2, r) __asm__ __volatile__("mov r0, %1\nmov r1, %2\nswi "#id"\n bx lr\nmov %0, r0":"=r"(r):"r"(p1), "r"(p2))
#define PLATFORM_SYSCALL3(id,p1,p2,p3,r) _PLATFORM_SYSCALL3(id,p1,p2,p3,r)
    #define _PLATFORM_SYSCALL3(id, p1, p2, p3, r) __asm__ __volatile__("mov r0, %1\nmov r1, %2\nmov r2, %3\nswi "#id"\n bx lr\nmov %0, r0":"=r"(r):"r"(p1), "r"(p2), "r"(p3))
	
	
#define __DEV_MAP

//Defined so we know the width of descriptors
typedef uint32_t pg_fld_t;
typedef uint32_t pg_sld_t;

#define PLATFORM_HAS_BIG_PAGES	//We can map memory with just a first order descriptor
#define BIG_PAGE_SIZE 	(1<<20)	//The resolution of that is 1MB
#define PAGE_SIZE		(1<<12) //The resolution of second level descriptors is 4kb so we have 256 second level
								//descriptors under each first order that isn't big
#define SO_PAGES		(BIG_PAGE_SIZE/PAGE_SIZE)
