#define PLATFORM_TOTAL_ADDR_RANGE ((void*)0xFFFFFFFF) //Highest 32 byte aligned address

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
	
#define PAGE_SIZE 4096
	
	
#define __DEV_MAP
