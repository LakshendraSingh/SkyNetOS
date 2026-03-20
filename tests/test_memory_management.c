#include "memory_management.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_page_allocation() {
    printf("--- Testing Page Allocation ---\n");
    init_memory_management();
    
    void* p1 = allocate_memory(4096);
    assert(p1 != NULL);
    printf("Allocated page 1 at %p\n", p1);
    
    void* p2 = allocate_memory(4096);
    assert(p2 != NULL);
    assert(p1 != p2);
    printf("Allocated page 2 at %p\n", p2);
    
    free_memory(p1);
    void* p3 = allocate_memory(4096);
    assert(p3 != NULL);
    assert(p3 != p2);
    printf("Allocated page 3 at %p (Next-Fit check)\n", p3);
    
    printf("  [PASS] test_page_allocation\n");
}

void test_heap_allocation() {
    printf("--- Testing Heap Allocation (malloc/free) ---\n");
    init_heap();
    
    char* s1 = (char*)malloc(10);
    assert(s1 != NULL);
    strcpy(s1, "Skynet");
    printf("Allocated 10 bytes: %s\n", s1);
    
    int* arr = (int*)malloc(100 * sizeof(int));
    assert(arr != NULL);
    for(int i=0; i<100; i++) arr[i] = i;
    assert(arr[99] == 99);
    printf("Allocated 100 ints and verified contents.\n");
    
    free(s1);
    free(arr);
    
    char* s2 = (char*)malloc(500);
    assert(s2 != NULL);
    printf("Allocated 500 bytes after free: %p\n", s2);
    free(s2);
    
    printf("  [PASS] test_heap_allocation\n");
}

int main() {
    printf("Starting Memory Management Unit Tests...\n");
    test_page_allocation();
    test_heap_allocation();
    printf("All memory management tests completed successfully!\n");
    return 0;
}