#include "arch/ARM11/cpu/cpu.h"
#include "kernel/cpu/cpu.h"
#include "arch/ARM11/cpu/coproc.h"
#include "kernel/mem/mem.h"

void cpu_set_user()
{
	uint32_t sp = (uint32_t)&sp;
	__asm__ __volatile__(
	"STR sp, [%0]\r\n"\
	"CPS #16\r\n"\
	"LDR sp, [%0]\r\n"\
	::"r"(sp):"r13","r14", "lr");
	mem_dsb();
}

uint32_t cpu_get_state()
{
	uint32_t res = 0;
	mem_dsb();
	mem_dmb();
	__asm__("mrs %0, cpsr":"=r"(res));
	mem_dsb();
	mem_dmb();
	return res;
}

uint32_t cpu_get_saved_state()
{
	uint32_t res = 0;
	mem_dmb();
	__asm__("mrs %0, spsr":"=r"(res));
	mem_dsb();
	return res;
}

uint32_t __attribute__((naked)) cpu_get_sp()
{
	__asm__(
	"mov r0, sp\n"
	"bx lr"
	);
}

uint32_t cpu_get_und_instr()
{
	uint32_t res = 0;
	mem_dmb();
	__asm__ __volatile__("mov %0, r14":"=r"(res));
	return res;
}

uint32_t cpu_get_ctrl_regs()
{
	uint32_t res = 0;
	mem_dmb();
	MRC(SYS_CTRL, 0, 1, 0, 0, res);
	mem_dsb();
	return res;
}

uint32_t cpu_get_secure_reg()
{
	uint32_t sys_val = 0;
	MRC(SYS_CTRL, 0, 1, 1, 0, sys_val);
	return sys_val;
}