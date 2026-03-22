#ifndef DISK_SCHEDULER_H
#define DISK_SCHEDULER_H

#include <stdbool.h>

#define MAX_DISK_REQUESTS 64

void init_disk_scheduler();
bool submit_disk_request(int track);
void process_disk_requests(int initial_head);

#endif
