#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__
#include <stdint.h>

static volatile uint32_t locked;

void init_spin_lock(uint32_t* lk);
void lock(uint32_t* lk);
void unlock(uint32_t* lk);

#endif
