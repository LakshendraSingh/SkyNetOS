#include "disk_scheduler.h"
#include "io.h"

#include "sync.h"
static spinlock_t disk_lock;

static int request_queue[MAX_DISK_REQUESTS];
static int queue_size = 0;

void init_disk_scheduler() {
    spinlock_init(&disk_lock);
    queue_size = 0;
}

bool submit_disk_request(int track) {
    spinlock_acquire(&disk_lock);
    if (queue_size >= MAX_DISK_REQUESTS) {
        spinlock_release(&disk_lock);
        return false;
    }
    request_queue[queue_size++] = track;
    spinlock_release(&disk_lock);
    return true;
}

static int abs_diff(int a, int b) {
    return (a > b) ? (a - b) : (b - a);
}

void process_disk_requests(int initial_head) {
    spinlock_acquire(&disk_lock);
    if (queue_size == 0) {
        print("No disk requests to process.\n");
        spinlock_release(&disk_lock);
        return;
    }

    int current_head = initial_head;
    int total_movement = 0;
    bool completed[MAX_DISK_REQUESTS];
    for (int i = 0; i < queue_size; i++) completed[i] = false;

    print("Disk Scheduling (SSTF) starting at track ");
    print_int(initial_head); print("\n");

    for (int count = 0; count < queue_size; count++) {
        int min_dist = 999999;
        int next_idx = -1;

        // Find shortest seek time
        for (int i = 0; i < queue_size; i++) {
            if (!completed[i]) {
                int dist = abs_diff(current_head, request_queue[i]);
                if (dist < min_dist) {
                    min_dist = dist;
                    next_idx = i;
                }
            }
        }

        if (next_idx != -1) {
            completed[next_idx] = true;
            print(" -> "); print_int(request_queue[next_idx]);
            total_movement += min_dist;
            current_head = request_queue[next_idx];
        }
    }
    print("\nTotal Head Movement: "); print_int(total_movement); print(" tracks.\n");
    queue_size = 0; // Clear queue after processing
    spinlock_release(&disk_lock);
}
