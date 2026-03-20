#ifndef SHA3_H
#define SHA3_H

#include <stddef.h>
#include <stdint.h>

/**
 * Computes the SHA-3 256 hash of a message.
 * @param msg Pointer to the input message.
 * @param len Length of the message in bytes.
 * @param hash Pointer to the buffer to store the 32-byte hash.
 */
void sha3_256(const uint8_t *msg, size_t len, uint8_t *hash);

#endif // SHA3_H
