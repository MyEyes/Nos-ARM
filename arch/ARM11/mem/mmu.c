#include "arch/ARM11/mem/mmu.h"
#include "arch/ARM11/cpu/coproc.h"
#ifdef MMU_DEBUG
#include "kernel/util/kernel_uart.h"
#endif

void mmu_enable()
{
	//Fetch old value of control register
	uint32_t old_val = 0;
	MRC(SYS_CTRL,0,MMU_SYSCTL_MAIN,0, 0, old_val);
	//Set mmu enable bit
	old_val |= 1;
	#ifdef MMU_DEBUG
	uart_puthex(old_val);
	#endif
	//Set new value
	MCR(SYS_CTRL,0,MMU_SYSCTL_MAIN,0, 0, old_val);
}

void mmu_disable()
{
	//Fetch old value of control register
	uint32_t old_val = 0;
	MRC(SYS_CTRL,0,MMU_SYSCTL_MAIN,0, 0, old_val);
	//Clear mmu enable bit
	old_val &= -2;
	#ifdef MMU_DEBUG
	uart_puthex(old_val);
	#endif
	//Set new value
	MCR(SYS_CTRL,0,MMU_SYSCTL_MAIN,0, 0, old_val);
}

void domain_manager_set()
{
	MCR(SYS_CTRL, 0, 3, 0, 0, 3);
}

void mmu_set_user_limit(uint32_t limit)
{
	char n = 7;
	limit >>=26; //Size in 32MB chunks 
	while((limit>>=2)>0)
		n--;
	mmu_set_ttbrc(n);
}

void mmu_set_user_pgtbl(void* tbl)
{
	mmu_set_ttbr0(tbl);
}

void mmu_set_kern_pgtbl(void* tbl)
{
	mmu_set_ttbr1(tbl);
}

void mmu_set_ttbrc(char n)
{
	#ifdef MMU_DEBUG
	uart_puthex(n);
	#endif
	MCR(SYS_CTRL,0,MMU_TTB_CTRL,0,MMU_TTBC, n);
}

void mmu_set_ttbr0(void* tbl_addr)
{
	uint32_t uaddr = (uint32_t)tbl_addr;
	uaddr &= (TTBR0_ADDR_MSK<<TTBR0_ADDR_OFFSET);
	#ifdef MMU_DEBUG
	uart_puthex(uaddr);
	#endif
	MCR(SYS_CTRL,0,MMU_TTB_CTRL,0,MMU_TTB0,uaddr);
}

void mmu_set_ttbr1(void* tbl_addr)
{
	uint32_t uaddr = (uint32_t)tbl_addr;
	uaddr &= (TTBR1_ADDR_MSK<<TTBR1_ADDR_OFFSET);
	#ifdef MMU_DEBUG
	uart_puthex(uaddr);
	#endif
	MCR(SYS_CTRL,0,MMU_TTB_CTRL,0,MMU_TTB1,uaddr);
}

void mem_dsb()
{
	MCR(SYS_CTRL,0,7,10,4,0);
}

void mem_dmb()
{
	MCR(SYS_CTRL,0,7,10,5,0);
}