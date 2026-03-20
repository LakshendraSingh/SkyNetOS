#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "security.h"

void test_sanitize_input() {
    char output[100];
    
    // Case 1: Normal input
    sanitize_input("hello", output, 100);
    assert(strcmp(output, "hello") == 0);
    
    // Case 2: Non-printable characters (scan codes or control chars)
    char input2[] = {'h', 'e', 1, 2, 'l', 'l', 'o', 0};
    sanitize_input(input2, output, 100);
    assert(strcmp(output, "hello") == 0);
    
    // Case 3: Empty string
    sanitize_input("", output, 100);
    assert(strcmp(output, "") == 0);
    
    // Case 4: Max length restriction (respects null terminator)
    sanitize_input("verylongstring", output, 5);
    assert(strcmp(output, "very") == 0);
    
    printf("  [PASS] test_sanitize_input\n");
}

void test_hash_password() {
    char hash1[MAX_PASSWORD_LEN];
    char hash2[MAX_PASSWORD_LEN];
    
    hash_password("skynet", hash1, MAX_PASSWORD_LEN);
    hash_password("skynet", hash2, MAX_PASSWORD_LEN);
    
    // Should be deterministic
    assert(strcmp(hash1, hash2) == 0);
    // SHA-3 256 hex string should be 64 characters
    assert(strlen(hash1) == 64);
    
    // Should be different for different inputs
    hash_password("admin", hash2, MAX_PASSWORD_LEN);
    assert(strcmp(hash1, hash2) != 0);
    
    // Should NOT be the same as plain text
    assert(strcmp(hash1, "skynet") != 0);
    
    printf("  [PASS] test_hash_password (SHA-3 256)\n");
}

void test_authenticate_user() {
    // Case 1: Correct credentials
    assert(authenticate_user("admin", "skynet") == 1);
    
    // Case 2: Incorrect username
    assert(authenticate_user("wronguser", "skynet") == 0);
    
    // Case 3: Incorrect password
    assert(authenticate_user("admin", "wrongpass") == 0);
    
    printf("  [PASS] test_authenticate_user (SHA-3 256)\n");
}

int main() {
    printf("Starting Security Module Unit Tests...\n");
    test_sanitize_input();
    test_hash_password();
    test_authenticate_user();
    printf("All security tests passed successfully!\n");
    return 0;
}
