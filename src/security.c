#include "security.h"
#include "sha3.h"
#include "string.h"
#include <stddef.h>

/**
 * Sanitizes input by removing non-printable characters.
 * @param input The original input string.
 * @param output The buffer to store sanitized output.
 * @param max_length Maximum size of the output buffer.
 */
void sanitize_input(const char* input, char* output, size_t max_length) {
    if (!input || !output || max_length == 0) return;
    
    size_t i = 0;
    size_t j = 0;
    
    while (input[i] != '\0' && j < max_length - 1) {
        // Only allow printable ASCII characters (32-126)
        if (input[i] >= 32 && input[i] <= 126) {
            output[j++] = input[i];
        }
        i++;
    }
    output[j] = '\0';
}

/**
 * SHA-3 256 hashing of a password.
 * @param password The raw password string.
 * @param hash The buffer to store the hex-encoded hash (min 65 bytes).
 * @param max_len Maximum size of the hash buffer.
 */
void hash_password(const char* password, char* hash, size_t max_len) {
    if (!password || !hash || max_len < 65) return;
    
    uint8_t raw_hash[32];
    sha3_256((const uint8_t*)password, strlen(password), raw_hash);
    
    const char hex_chars[] = "0123456789abcdef";
    for (int i = 0; i < 32; i++) {
        hash[i * 2] = hex_chars[raw_hash[i] >> 4];
        hash[i * 2 + 1] = hex_chars[raw_hash[i] & 0x0F];
    }
    hash[64] = '\0';
}

/**
 * Authenticates a user against hardcoded credentials using SHA-3 256.
 * @param username The entered username.
 * @param password The entered password.
 * @return 1 on success, 0 on failure.
 */
int authenticate_user(const char* username, const char* password) {
    const char* admin_user = "admin";
    const char* admin_pass_raw = "skynet";
    
    char stored_hash[MAX_PASSWORD_LEN];
    char input_hash[MAX_PASSWORD_LEN];
    
    // Hash the hardcoded password (simulating checking a stored hash)
    hash_password(admin_pass_raw, stored_hash, MAX_PASSWORD_LEN);
    
    // Hash the input password to compare
    hash_password(password, input_hash, MAX_PASSWORD_LEN);
    
    if (strcmp(username, admin_user) == 0 && strcmp(input_hash, stored_hash) == 0) {
        return 1;
    }
    
    return 0;
}
