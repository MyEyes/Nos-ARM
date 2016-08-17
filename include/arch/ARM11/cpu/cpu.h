#ifndef __A_CPU_H__
#define __A_CPU_H__

#define CPU_INT_STACK_SIZE 256

#define CPU_MODE_USR 0x10 //0b10000
#define CPU_MODE_FIQ 0x11 //0b10001
#define CPU_MODE_IRQ 0x12 //0b10010
#define CPU_MODE_SVC 0x13 //0b10011
#define CPU_MODE_ABT 0x17 //0b10111
#define CPU_MODE_UND 0x1B //0b11011
#define CPU_MODE_SYS 0x1F //0b11111

#endif