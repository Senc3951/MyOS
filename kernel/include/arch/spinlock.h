#pragma once

#include <common.h>

typedef struct SPINLOCK
{
    uint32_t locked;
} spinlock_t;

#define SPINLOCK_INLINE_INIT() { .locked = 0 }
inline void spinlock_init(spinlock_t *lock) { lock->locked = 0; }

void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);