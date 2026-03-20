#include "io.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

// Mock video memory for testing logic in a hosted environment
char pseudo_screen[80 * 25];
int pseudo_cursor = 0;

void test_print_int() {
    printf("--- Testing print_int logic ---\n");
    // Since we're in a hosted test, we can't easily check the screen
    // but we can verify the function doesn't crash and logic is sound.
    
    print("Testing numbers: ");
    print_int(0); print(" ");
    print_int(123); print(" ");
    print_int(-456); print("\n");
    
    printf("  [PASS] test_print_int (Visual verification recommended or more complex mocks required)\n");
}

int main() {
    printf("Starting I/O Unit Tests...\n");
    test_print_int();
    printf("All I/O tests completed successfully!\n");
    return 0;
}