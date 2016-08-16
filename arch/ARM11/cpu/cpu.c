#include "arch/ARM11/cpu/cpu.h"
#include "kernel/cpu/cpu.h"

void cpu_set_user()
{
	uint32_t sp = (uint32_t)&sp;
	__asm__ __volatile__(
	"STR sp, [%0]\r\n"\
	"CPS #16\r\n"
	"LDR sp, [%0]\r\n"
	::"r"(sp):"r13","r14", "lr");
}

uint32_t cpu_get_state()
{
	uint32_t res = 0;
	__asm__("mrs %0, cpsr":"=r"(res));
	return res;
}

uint32_t cpu_get_saved_state()
{
	uint32_t res = 0;
	__asm__("mrs %0, spsr":"=r"(res));
	return res;
}

uint32_t cpu_get_und_instr()
{
	uint32_t res = 0;
	__asm__ __volatile__("mov %0, r14":"=r"(res));
	return res;
}