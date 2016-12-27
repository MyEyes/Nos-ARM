#ifndef __RESET_H__
#define __RESET_H__

#include __PLATFORM__

#define WATCHDOG_ADDR (PERIPHERALS_LOW_ADDR + 0x100000)
#define RSTC 0x1C
#define WD 0x24

#endif
