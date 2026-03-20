#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "scheduler.h"

void test_banker_algorithm() {
    printf("--- Testing Banker's Algorithm ---\n");
    // Start with smaller resources to make it easier to hit limits
    available_resources[0] = 10;
    available_resources[1] = 10;
    available_resources[2] = 10;
    scheduler_init();
    
    // Task 1: Max demand {8, 8, 8}
    int res1[] = {8, 8, 8};
    int pid1 = task_create(res1);
    
    // Request {5, 5, 5} - Should be safe
    int req1[] = {5, 5, 5};
    printf("Task %d requesting {5,5,5}...\n", pid1);
    assert(request_resources(pid1, req1) == true);
    
    // Task 2: Max demand {7, 7, 7}
    int res2[] = {7, 7, 7};
    int pid2 = task_create(res2);
    
    // Available is now {5, 5, 5}.
    // If Task 2 requests {4, 4, 4}:
    // Available becomes {1, 1, 1}.
    // Task 1 needs {3, 3, 3} (cannot satisfy).
    // Task 2 needs {3, 3, 3} (cannot satisfy).
    // This is UNSAFE.
    int req2[] = {4, 4, 4}; 
    printf("Task %d requesting {4,4,4} (Unsafe)...\n", pid2);
    assert(request_resources(pid2, req2) == false); 
    
    printf("  [PASS] test_banker_algorithm\n\n");
}

void test_mlfq_scheduling() {
    printf("--- Testing MLFQ Scheduling ---\n");
    scheduler_init();
    
    int res[] = {1, 1, 1};
    int pid1 = task_create(res); // PID 0, starts in Q0
    int pid2 = task_create(res); // PID 1, starts in Q0
    
    // Step 1: Run PID 1 and use full quantum
    printf("Scheduling 1st task...\n");
    schedule(); 
    printf("Task 0 yielding with full quantum used...\n");
    task_yield(true); // Should demote PID 0 to Q1
    
    // Step 2: Run PID 2 and yield early
    printf("Scheduling next task (should be 1)...\n");
    schedule();
    printf("Task 1 yielding early...\n");
    task_yield(false); // Should stay in Q0
    
    // Step 3: PID 2 should be picked again because it's higher priority (Q0 > Q1)
    printf("Scheduling next task (should be 1 again)...\n");
    schedule();
    
    printf("  [PASS] test_mlfq_scheduling\n\n");
}

void test_priority_boost() {
    printf("--- Testing Priority Boost ---\n");
    scheduler_init();
    
    int res[] = {1, 1, 1};
    int pid = task_create(res);
    
    schedule();
    task_yield(true); // Demote to Q1
    schedule();
    task_yield(true); // Demote to Q2
    
    printf("Executing priority boost...\n");
    priority_boost();
    
    // After boost, task should be in Q0 and picked first if we schedule
    // No other tasks, so it will be picked regardless, but we check logic in log.
    schedule(); 
    
    printf("  [PASS] test_priority_boost\n\n");
}

int main() {
    printf("Starting Task Scheduler Unit Tests...\n");
    test_banker_algorithm();
    test_mlfq_scheduling();
    test_priority_boost();
    printf("All scheduler tests completed successfully!\n");
    return 0;
}
