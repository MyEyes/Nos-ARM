#include "usr/std/spinlock.h"
#include <stdint.h>
#include <lock.h>
#include __PLATFORM__

void spinlock_init(spinlock_t* lk)
{
	*lk = 0;
}

void spinlock_lock(spinlock_t* lk)
{
	while(__sync_lock_test_and_set(lk, 1)!=0)
	{//Spin until sync lock test and set returns non zero value
	}
}

void spinlock_unlock(spinlock_t* lk)
{
	__sync_lock_release(lk);
}