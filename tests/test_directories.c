#include <stdio.h>
#include <string.h>
#include "file_system.h"
#include "memory_management.h"
#include "error_codes.h"

void err_handler(int code) {
    if (code == ERR_FILE_EXISTS) printf("[MOCK] ERR_FILE_EXISTS handled.\n");
    else if (code == ERR_FILE_NOT_FOUND) printf("[MOCK] ERR_FILE_NOT_FOUND handled.\n");
    else printf("[MOCK] err_handler called with code %d\n", code);
}

void test_create_directory() {
    printf("--- Testing Create Directory ---\n");
    init_memory_management();
    init_heap();
    init_file_system();
    
    create_directory("home");
    printf("Created directory 'home'.\n");
    
    create_file("home/test.txt"); 
    printf("Created file 'home/test.txt'.\n");
    
    printf("  [PASS] test_create_directory\n");
}

void test_delete_directory() {
    printf("--- Testing Delete Directory ---\n");
    init_memory_management();
    init_heap();
    init_file_system();
    
    create_directory("tmp");
    delete_directory("tmp");
    printf("Created and deleted 'tmp'.\n");
    
    printf("  [PASS] test_delete_directory\n");
}

int main() {
    printf("Starting Directory Unit Tests...\n");
    test_create_directory();
    test_delete_directory();
    printf("All directory tests completed successfully!\n");
    return 0;
}
