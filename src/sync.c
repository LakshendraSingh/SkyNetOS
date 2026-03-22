#include "sync.h"
#include "scheduler.h"

void spinlock_init(spinlock_t* lock) {
    lock->locked = 0;
}

void spinlock_acquire(spinlock_t* lock) {
    while (__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE)) {
        while (lock->locked) {
#if defined(__i386__) || defined(__x86_64__)
            __asm__ volatile("pause" ::: "memory");
#elif defined(__aarch64__)
            __asm__ volatile("yield" ::: "memory");
#else
            __asm__ volatile("" ::: "memory");
#endif
        }
    }
}

void spinlock_release(spinlock_t* lock) {
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
}

void mutex_init(mutex_t* m) {
    spinlock_init(&m->lock);
    m->owner_pid = -1;
    m->queue_head = 0;
    m->queue_tail = 0;
}

void mutex_lock(mutex_t* m) {
    int pid = get_current_pid();
    if (pid == -1) return; // Ignore if called without active task

    spinlock_acquire(&m->lock);
    if (m->owner_pid == -1) {
        m->owner_pid = pid;
        spinlock_release(&m->lock);
    } else {
        // Add to blocked queue
        if ((m->queue_tail + 1) % MAX_BLOCKED_QUEUE != m->queue_head) {
            m->blocked_queue[m->queue_tail] = pid;
            m->queue_tail = (m->queue_tail + 1) % MAX_BLOCKED_QUEUE;
        }
        spinlock_release(&m->lock);
        task_block(pid);
        task_yield(false); 
    }
}

void mutex_unlock(mutex_t* m) {
    spinlock_acquire(&m->lock);
    if (m->queue_head != m->queue_tail) {
        int next_pid = m->blocked_queue[m->queue_head];
        m->queue_head = (m->queue_head + 1) % MAX_BLOCKED_QUEUE;
        m->owner_pid = next_pid;
        task_unblock(next_pid);
    } else {
        m->owner_pid = -1;
    }
    spinlock_release(&m->lock);
}
