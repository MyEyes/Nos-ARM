#ifndef __SCHD_H__
#define __SCHD_H__

#include "kernel/proc/thread.h"

void schd_init();
void schd_chg_thread();
void schd_add_thread(thread_t*);

#endif
