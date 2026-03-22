#include <stdio.h>
#include <assert.h>
#include "disk_scheduler.h"

void test_disk_scheduling() {
    printf("--- Testing Disk Scheduling (SSTF) ---\n");
    init_disk_scheduler();
    
    assert(submit_disk_request(98) == true);
    assert(submit_disk_request(183) == true);
    assert(submit_disk_request(37) == true);
    assert(submit_disk_request(122) == true);
    assert(submit_disk_request(14) == true);
    assert(submit_disk_request(124) == true);
    assert(submit_disk_request(65) == true);
    assert(submit_disk_request(67) == true);
    
    // We only test that it processes without errors since it prints to console directly.
    process_disk_requests(53);
    
    printf("  [PASS] test_disk_scheduling\n\n");
}

int main() {
    printf("Starting Disk Scheduler Unit Tests...\n");
    test_disk_scheduling();
    printf("All disk scheduler tests completed successfully!\n");
    return 0;
}
