#include "scheduler.h"
#include "io.h"
#include "string.h"

// System state
static tcb_t tasks[MAX_TASKS];
static int task_count = 0;
static int current_pid = -1;

// MLFQ Queues
static int queues[NUM_QUEUES][MAX_TASKS];
static int queue_counts[NUM_QUEUES];
static int quantums[NUM_QUEUES] = {4, 8, 16};

// Resource Management
int available_resources[MAX_RESOURCES] = {10, 20, 10}; // Total system resources

void scheduler_init() {
    memset(tasks, 0, sizeof(tasks));
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].pid = -1;
        tasks[i].state = TASK_TERMINATED;
    }
    for (int i = 0; i < NUM_QUEUES; i++) {
        queue_counts[i] = 0;
    }
    print("Scheduler initialized with MLFQ and Banker's Algorithm.\n");
}

int task_create(int max_resources[MAX_RESOURCES]) {
    if (task_count >= MAX_TASKS) return -1;

    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_TERMINATED) {
            tasks[i].pid = i;
            tasks[i].state = TASK_READY;
            tasks[i].priority_level = 0; // New tasks start at highest priority
            tasks[i].remaining_quantum = quantums[0];
            tasks[i].time_slice = quantums[0];
            
            tasks[i].thread_count = 1;
            tasks[i].msg_count = 0;
            mutex_init(&tasks[i].ipc_mutex);
            for(int m = 0; m < MAX_IPC_MESSAGES; m++) {
                tasks[i].mailbox[m].valid = false;
            }
            
            for (int r = 0; r < MAX_RESOURCES; r++) {
                tasks[i].max_demand[r] = max_resources[r];
                tasks[i].allocation[r] = 0;
                tasks[i].need[r] = max_resources[r];
            }
            
            // Add to Q0
            queues[0][queue_counts[0]++] = tasks[i].pid;
            task_count++;
            print("Task "); print_int(tasks[i].pid); print(" created and added to Q0.\n");
            return tasks[i].pid;
        }
    }
    return -1;
}

void task_terminate(int pid) {
    if (pid < 0 || pid >= MAX_TASKS || tasks[pid].state == TASK_TERMINATED) return;

    // Release all resources
    for (int r = 0; r < MAX_RESOURCES; r++) {
        available_resources[r] += tasks[pid].allocation[r];
        tasks[pid].allocation[r] = 0;
    }
    
    tasks[pid].state = TASK_TERMINATED;
    task_count--;
    print("Task "); print_int(pid); print(" terminated and resources reclaimed.\n");
}

bool is_safe_state() {
    int work[MAX_RESOURCES];
    bool finish[MAX_TASKS];
    memcpy(work, available_resources, sizeof(work));
    memset(finish, false, sizeof(finish));

    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_TERMINATED) finish[i] = true;
    }

    int count = 0;
    while (count < task_count) {
        bool found = false;
        for (int i = 0; i < MAX_TASKS; i++) {
            if (!finish[i]) {
                bool can_satisfy = true;
                for (int r = 0; r < MAX_RESOURCES; r++) {
                    if (tasks[i].need[r] > work[r]) {
                        can_satisfy = false;
                        break;
                    }
                }
                if (can_satisfy) {
                    for (int r = 0; r < MAX_RESOURCES; r++) {
                        work[r] += tasks[i].allocation[r];
                    }
                    finish[i] = true;
                    found = true;
                    count++;
                }
            }
        }
        if (!found) break;
    }

    for (int i = 0; i < MAX_TASKS; i++) {
        if (!finish[i]) return false;
    }
    return true;
}

bool request_resources(int pid, int request[MAX_RESOURCES]) {
    if (pid < 0 || pid >= MAX_TASKS || tasks[pid].state == TASK_TERMINATED) return false;

    // Consistency checks
    for (int r = 0; r < MAX_RESOURCES; r++) {
        if (request[r] > tasks[pid].need[r] || request[r] > available_resources[r]) {
            return false;
        }
    }

    // Trial allocation
    for (int r = 0; r < MAX_RESOURCES; r++) {
        available_resources[r] -= request[r];
        tasks[pid].allocation[r] += request[r];
        tasks[pid].need[r] -= request[r];
    }

    if (is_safe_state()) {
        print("Task "); print_int(pid); print(" granted resources.\n");
        return true;
    } else {
        // Rollback
        for (int r = 0; r < MAX_RESOURCES; r++) {
            available_resources[r] += request[r];
            tasks[pid].allocation[r] -= request[r];
            tasks[pid].need[r] += request[r];
        }
        print("Task "); print_int(pid); print(" denied resources (Deadlock Avoided).\n");
        return false;
    }
}

void release_resources(int pid, int release[MAX_RESOURCES]) {
    if (pid < 0 || pid >= MAX_TASKS || tasks[pid].state == TASK_TERMINATED) return;

    for (int r = 0; r < MAX_RESOURCES; r++) {
        if (release[r] > tasks[pid].allocation[r]) continue;
        available_resources[r] += release[r];
        tasks[pid].allocation[r] -= release[r];
        tasks[pid].need[r] += release[r];
    }
    print("Task "); print_int(pid); print(" released resources.\n");
}

void schedule() {
    int next_pid = -1;

    // Pick task from highest priority queue first
    for (int q = 0; q < NUM_QUEUES; q++) {
        if (queue_counts[q] > 0) {
            next_pid = queues[q][0];
            // Rotate the queue (Round Robin within the same level)
            for (int i = 0; i < queue_counts[q] - 1; i++) {
                queues[q][i] = queues[q][i + 1];
            }
            queue_counts[q]--;
            break;
        }
    }

    if (next_pid != -1) {
        current_pid = next_pid;
        tasks[current_pid].state = TASK_RUNNING;
        print("Switching to Task "); print_int(current_pid); print(" (Queue "); print_int(tasks[current_pid].priority_level); print(").\n");
    } else {
        print("No tasks ready to run.\n");
    }
}

void task_yield(bool used_full_quantum) {
    if (current_pid == -1) return;

    tasks[current_pid].state = TASK_READY;

    if (used_full_quantum) {
        // Demote to next queue if not already at lowest
        if (tasks[current_pid].priority_level < NUM_QUEUES - 1) {
            tasks[current_pid].priority_level++;
            tasks[current_pid].remaining_quantum = quantums[tasks[current_pid].priority_level];
        }
    }
    
    // Add back to its current priority queue
    int q = tasks[current_pid].priority_level;
    queues[q][queue_counts[q]++] = current_pid;
    
    print("Task "); print_int(current_pid); print(" yielded. Next in Queue "); print_int(q); print(".\n");
    current_pid = -1;
    schedule();
}

void priority_boost() {
    print("Priority Boost: Moving all tasks to Q0.\n");
    for (int q = 1; q < NUM_QUEUES; q++) {
        for (int i = 0; i < queue_counts[q]; i++) {
            int pid = queues[q][i];
            tasks[pid].priority_level = 0;
            tasks[pid].remaining_quantum = quantums[0];
            queues[0][queue_counts[0]++] = pid;
        }
        queue_counts[q] = 0;
    }
}

bool get_task_info(int pid, tcb_t* out) {
    if (pid < 0 || pid >= MAX_TASKS || tasks[pid].state == TASK_TERMINATED) return false;
    if (out) *out = tasks[pid];
    return true;
}

void system_tick() {
    if (current_pid != -1) {
        tasks[current_pid].remaining_quantum--;
        if (tasks[current_pid].remaining_quantum <= 0) {
            task_yield(true);
        }
    } else {
        schedule();
    }
}

bool thread_create(int pid) {
    if (pid < 0 || pid >= MAX_TASKS || tasks[pid].state == TASK_TERMINATED) return false;
    if (tasks[pid].thread_count >= MAX_THREADS_PER_TASK) return false;
    tasks[pid].thread_count++;
    print("Thread created in Task "); print_int(pid); print(" (Total: "); print_int(tasks[pid].thread_count); print(")\n");
    return true;
}

bool ipc_send(int receiver_pid, const char* msg) {
    if (receiver_pid < 0 || receiver_pid >= MAX_TASKS || tasks[receiver_pid].state == TASK_TERMINATED) return false;
    tcb_t* t = &tasks[receiver_pid];
    
    mutex_lock(&t->ipc_mutex);
    if (t->msg_count >= MAX_IPC_MESSAGES) {
        mutex_unlock(&t->ipc_mutex);
        return false;
    }
    
    for (int i = 0; i < MAX_IPC_MESSAGES; i++) {
        if (!t->mailbox[i].valid) {
            int j = 0;
            while(msg[j] && j < MAX_MSG_LENGTH - 1) {
                t->mailbox[i].message[j] = msg[j];
                j++;
            }
            t->mailbox[i].message[j] = '\0';
            t->mailbox[i].valid = true;
            t->msg_count++;
            mutex_unlock(&t->ipc_mutex);
            print("Message sent to Task "); print_int(receiver_pid); print("\n");
            return true;
        }
    }
    mutex_unlock(&t->ipc_mutex);
    return false;
}

bool ipc_receive(int receiver_pid, char* buffer_out) {
    if (receiver_pid < 0 || receiver_pid >= MAX_TASKS || tasks[receiver_pid].state == TASK_TERMINATED) return false;
    tcb_t* t = &tasks[receiver_pid];
    
    mutex_lock(&t->ipc_mutex);
    if (t->msg_count == 0) {
        mutex_unlock(&t->ipc_mutex);
        return false;
    }
    
    for (int i = 0; i < MAX_IPC_MESSAGES; i++) {
        if (t->mailbox[i].valid) {
            int j = 0;
            while(t->mailbox[i].message[j] && j < MAX_MSG_LENGTH - 1) {
                buffer_out[j] = t->mailbox[i].message[j];
                j++;
            }
            buffer_out[j] = '\0';
            t->mailbox[i].valid = false;
            t->msg_count--;
            mutex_unlock(&t->ipc_mutex);
            return true;
        }
    }
    mutex_unlock(&t->ipc_mutex);
    return false;
}

int get_current_pid() {
    return current_pid;
}

void task_block(int pid) {
    if (pid < 0 || pid >= MAX_TASKS || tasks[pid].state == TASK_TERMINATED) return;
    tasks[pid].state = TASK_BLOCKED;
}

void task_unblock(int pid) {
    if (pid < 0 || pid >= MAX_TASKS || tasks[pid].state != TASK_BLOCKED) return;
    tasks[pid].state = TASK_READY;
    int q = tasks[pid].priority_level;
    queues[q][queue_counts[q]++] = pid;
}
