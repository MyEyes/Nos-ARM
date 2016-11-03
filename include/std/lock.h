#ifndef __LOCK_H__
#define __LOCK_H__

typedef unsigned char spinlock_t;
void spinlock_init(spinlock_t* lk);

void spinlock_lock(spinlock_t* lk);

void spinlock_unlock(spinlock_t* lk);
#endif