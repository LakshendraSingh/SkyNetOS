#include "sha3.h"
#include "string.h"

// Keccak-f[1600] state size is 1600 bits = 200 bytes = 25 uint64_t
#define KECCAK_STATE_WORDS 25

// SHA-3 256: rate = 1088 bits (136 bytes), capacity = 512 bits (64 bytes)
#define SHA3_256_RATE 136

// Keccak-f[1600] round constants
static const uint64_t keccakf_rndc[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL, 0x000000008000808aULL,
    0x0000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
    0x0000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
    0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
    0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
    0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
    0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

// Rotation constants
static const unsigned int keccakf_rotc[24] = {
    1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
    27, 44, 65, 8,  25, 43, 62, 18, 39, 61, 20, 44
};

// Lane mapping
static const unsigned int keccakf_piln[24] = {
    10, 7,  11, 17, 18, 3,  5,  16, 8,  21, 24, 4,
    15, 23, 19, 13, 12, 2,  20, 14, 22, 9,  6,  1
};

#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))

// Keccak-f[1600] permutation
static void keccakf(uint64_t st[25]) {
    int i, j, r;
    uint64_t t, bc[5];

    for (r = 0; r < 24; r++) {
        // Theta step
        for (i = 0; i < 5; i++)
            bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];
        for (i = 0; i < 5; i++) {
            t = bc[(i + 4) % 5] ^ ROTL64(bc[(i + 1) % 5], 1);
            for (j = 0; j < 25; j += 5) st[j + i] ^= t;
        }

        // Rho and Pi steps
        t = st[1];
        for (i = 0; i < 24; i++) {
            j = keccakf_piln[i];
            bc[0] = st[j];
            st[j] = ROTL64(t, keccakf_rotc[i]);
            t = bc[0];
        }

        // Chi step
        for (j = 0; j < 25; j += 5) {
            for (i = 0; i < 5; i++) bc[i] = st[j + i];
            for (i = 0; i < 5; i++)
                st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
        }

        // Iota step
        st[0] ^= keccakf_rndc[r];
    }
}

void sha3_256(const uint8_t *msg, size_t len, uint8_t *hash) {
    uint64_t st[KECCAK_STATE_WORDS];
    uint8_t temp[SHA3_256_RATE];
    size_t i, j;

    memset(st, 0, sizeof(st));

    // Absorb phase
    while (len >= SHA3_256_RATE) {
        for (i = 0; i < SHA3_256_RATE / 8; i++) {
            uint64_t lane = 0;
            for (j = 0; j < 8; j++) {
                lane |= (uint64_t)msg[i * 8 + j] << (8 * j);
            }
            st[i] ^= lane;
        }
        keccakf(st);
        msg += SHA3_256_RATE;
        len -= SHA3_256_RATE;
    }

    // Padding
    memset(temp, 0, SHA3_256_RATE);
    memcpy(temp, msg, len);
    temp[len] = 0x06; // SHA-3 delimiter
    temp[SHA3_256_RATE - 1] |= 0x80;

    for (i = 0; i < SHA3_256_RATE / 8; i++) {
        uint64_t lane = 0;
        for (j = 0; j < 8; j++) {
            lane |= (uint64_t)temp[i * 8 + j] << (8 * j);
        }
        st[i] ^= lane;
    }
    keccakf(st);

    // Squeeze phase (only one block needed for 256 bits)
    for (i = 0; i < 4; i++) { // 4 * 8 = 32 bytes (256 bits)
        for (j = 0; j < 8; j++) {
            hash[i * 8 + j] = (uint8_t)(st[i] >> (8 * j));
        }
    }
}
