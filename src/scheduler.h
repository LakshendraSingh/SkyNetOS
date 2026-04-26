#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "sync.h"

#define MAX_TASKS 16
#define MAX_RESOURCES 3
#define NUM_QUEUES 3
#define MAX_THREADS_PER_TASK 4
#define MAX_IPC_MESSAGES 5
#define MAX_MSG_LENGTH 64

// Resource indices for Banker's Algorithm
#define RES_CPU    0
#define RES_MEMORY 1
#define RES_IO     2

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_TERMINATED
} task_state_t;

typedef struct {
    char message[MAX_MSG_LENGTH];
    bool valid;
} ipc_message_t;

typedef struct {
    int pid;
    int priority_level;   // Current MLFQ queue (0 = Highest Priority)
    task_state_t state;
    int remaining_quantum;
    int time_slice;       // Total time slice for the current priority level
    int remaining_work;   // Simulated work units left (-1 = infinite/manual)
    
    int max_demand[MAX_RESOURCES];
    int allocation[MAX_RESOURCES];
    int need[MAX_RESOURCES];
    
    int thread_count;
    ipc_message_t mailbox[MAX_IPC_MESSAGES];
    int msg_count;
    mutex_t ipc_mutex;
} tcb_t;

// Resource management
extern int available_resources[MAX_RESOURCES];

/**
 * Initializes the scheduler, ready queues, and system resources.
 */
void scheduler_init();

/**
 * Creates a new task and adds it to the highest priority queue.
 * @param max_resources Array of maximum resource needs for this task.
 * @return PID of the new task or -1 if full.
 */
int task_create(int max_resources[MAX_RESOURCES]);

/**
 * Terminates a task and releases all its resources.
 * @param pid The process ID to terminate.
 */
void task_terminate(int pid);

/**
 * The main scheduling function. Picks the next task to run based on MLFQ.
 */
void schedule();

/**
 * Called when a task yields or its quantum expires.
 * @param used_full_quantum True if the task used its entire time slice.
 */
void task_yield(bool used_full_quantum);

/**
 * Requests resources using Dijkstra's Banker's Algorithm.
 * @param pid The process ID requesting resources.
 * @param request Array of requested resource amounts.
 * @return True if granted (safe), false if denied (unsafe or unavailable).
 */
bool request_resources(int pid, int request[MAX_RESOURCES]);

/**
 * Releases resources held by a task.
 * @param pid The process ID releasing resources.
 * @param release Array of resource amounts to release.
 */
void release_resources(int pid, int release[MAX_RESOURCES]);

/**
 * Checks if the system is in a safe state as per Banker's Algorithm.
 * @return True if safe, false if unsafe.
 */
bool is_safe_state();

/**
 * Periodically boosts all tasks to the highest priority queue to prevent starvation.
 */
void priority_boost();

/**
 * Retrieves information about a task.
 * @param pid The process ID.
 * @param out Pointer to TCB to fill.
 * @return True if found, false otherwise.
 */
bool get_task_info(int pid, tcb_t* out);

/**
 * System tick to simulate time passing for background processes.
 */
void system_tick();

/**
 * Add a thread to an existing process.
 */
bool thread_create(int pid);

/**
 * Send an IPC message to a process.
 */
bool ipc_send(int receiver_pid, const char* msg);

/**
 * Retrieve the oldest IPC message for a process.
 */
bool ipc_receive(int receiver_pid, char* buffer_out);

int get_current_pid();
void task_block(int pid);
void task_unblock(int pid);

/**
 * Set the remaining work units for a task.
 * The task auto-terminates when work reaches 0.
 * Set to -1 for infinite (manual termination only).
 */
void task_set_work(int pid, int work_units);

#endif // SCHEDULER_H
