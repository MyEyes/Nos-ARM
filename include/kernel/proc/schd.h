#ifndef __SCHD_H__
#define __SCHD_H__

#include "kernel/proc/thread.h"
#include "kernel/proc/threadqueue.h"
#define SCHD_NUM_PRIORITIES 7

void schd_init();
void schd_chg_thread();
void schd_add_thread(thread_t*);
thread_node_t* schd_get_empty_node();

#endif