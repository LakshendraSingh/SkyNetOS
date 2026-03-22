#ifndef SYNC_H
#define SYNC_H

#include <stdbool.h>

/* Atomic Spinlocks */
typedef struct {
    volatile int locked;
} spinlock_t;

void spinlock_init(spinlock_t* lock);
void spinlock_acquire(spinlock_t* lock);
void spinlock_release(spinlock_t* lock);

/* Blocking Mutexes */
#define MAX_BLOCKED_QUEUE 16

typedef struct {
    spinlock_t lock;
    int owner_pid;
    int blocked_queue[MAX_BLOCKED_QUEUE];
    int queue_head;
    int queue_tail;
} mutex_t;

void mutex_init(mutex_t* m);
void mutex_lock(mutex_t* m);
void mutex_unlock(mutex_t* m);

#endif
