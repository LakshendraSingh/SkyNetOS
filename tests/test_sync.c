#include <stdio.h>
#include <assert.h>
#include "sync.h"
#include "scheduler.h"

void test_spinlock() {
    printf("--- Testing Spinlock ---\n");
    spinlock_t lock;
    spinlock_init(&lock);
    
    assert(lock.locked == 0);
    spinlock_acquire(&lock);
    assert(lock.locked != 0);
    spinlock_release(&lock);
    assert(lock.locked == 0);
    
    printf("  [PASS] test_spinlock\n\n");
}

void test_mutex() {
    printf("--- Testing Mutex ---\n");
    scheduler_init(); 
    
    int res[] = {1,1,1};
    int pid1 = task_create(res);
    schedule(); // current_pid = pid1
    
    mutex_t m;
    mutex_init(&m);
    
    mutex_lock(&m);
    assert(m.owner_pid == pid1);
    
    mutex_unlock(&m);
    assert(m.owner_pid == -1);
    
    printf("  [PASS] test_mutex\n\n");
}

int main() {
    printf("Starting Sync Primitive Tests...\n");
    test_spinlock();
    test_mutex();
    printf("All sync tests completed successfully!\n");
    return 0;
}
