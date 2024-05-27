#include <arch/spinlock.h>
#include <arch/cpu.h>

void spinlock_acquire(spinlock_t *lock)
{
    // xchg is atomic
    while (__xchg(&lock->locked, 1) != 0) ;

    // Tell the C compiler and the processor to not move loads or stores
    // past this point, to ensure that the critical section's memory
    // references happen after the lock is acquired.
    __sync_synchronize();
}

void spinlock_release(spinlock_t *lock)
{
    // Tell the C compiler and the processor to not move loads or stores
    // past this point, to ensure that all the stores in the critical
    // section are visible to other cores before the lock is released.
    // Both the C compiler and the hardware may re-order loads and
    // stores; __sync_synchronize() tells them both not to.
    __sync_synchronize();
    
    // Release the lock
    asm volatile("movl $0, %0" : "+m" (lock->locked) : );
}