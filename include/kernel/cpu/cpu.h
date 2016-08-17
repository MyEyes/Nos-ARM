#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>

void cpu_set_user();
uint32_t cpu_get_state();
uint32_t cpu_get_saved_state();
uint32_t cpu_get_und_instr();
uint32_t cpu_get_ctrl_regs();

#endif