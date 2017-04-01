#ifndef __INT_H__
#define __INT_H__

#define INT_PHYS_ADDR 0x0 //0
#define INT_HIGH_VEC_ADDR 0xFFFF0000

#define INT_RST 0
#define INT_UND 0x4
#define INT_SWI 0x8
#define INT_PAB 0xC
#define INT_DAB 0x10
#define INT_IRQ 0x18
#define INT_FIQ 0x1C

#define INT_ADDR_LOC 0x20
#define LDR_ADDR_OFF (INT_ADDR_LOC-8)

#define INT_HND_ENTRY __asm__(\
        "SRS #15\n"\
        "stmfd sp!, {r0-r12}\n"\
        "mov r1, sp\n"\
        "CPS #19\n"\
        );

#define INT_HND_RETURN __asm__(\
        "CPS #15\n"\
        "mov sp, r0\n"\
        "ldmfd sp!, {r0-r12}\n"\
        "RFE sp\n"\
        );
#endif
