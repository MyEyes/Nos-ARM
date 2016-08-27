#include "usr/std/spinlock.h"
#include <stdint.h>

void init_spin_lock(uint32_t* lk)
{
	*lk = 0;
}

void lock(uint32_t* lk)
{
	while(__sync_lock_test_and_set(lk, 1))
		;
}

void unlock(uint32_t* lk)
{
	*lk = 0;
}