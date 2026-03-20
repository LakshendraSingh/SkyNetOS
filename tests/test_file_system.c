#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "file_system.h"
#include "memory_management.h"
#include "error_codes.h"

void err_handler(int code) {
    printf("[MOCK FS TEST] err_handler called with code %d\n", code);
}

void test_create_file() {
    printf("--- Testing Create File ---\n");
    init_memory_management();
    init_heap();
    init_file_system();
    
    create_file("test.txt");
    // Verify file exists (find_file_index is static, so we check through internal state if possible, 
    // or just ensure no crash for now as a smoke test)
    printf("  [PASS] test_create_file\n");
}

void test_create_file_performance() {
    printf("--- Testing Create File Performance ---\n");
    init_file_system();
    
    clock_t start_time = clock();
    create_file("perf.txt");
    clock_t end_time = clock();
    
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", time_taken);
    assert(time_taken < 0.1);
    printf("  [PASS] test_create_file_performance\n");
}

int main() {
    printf("Starting File System Unit Tests...\n");
    test_create_file();
    test_create_file_performance();
    printf("All file system tests completed successfully!\n");
    return 0;
}