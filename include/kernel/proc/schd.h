#ifndef __SCHD_H__
#define __SCHD_H__

#include "kernel/proc/thread.h"
extern thread_t* curr_thread;

void schd_init();

#endif