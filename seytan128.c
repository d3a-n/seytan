#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Print a 128-bit number as exactly 32 hexadecimal digits.
void print_uint128_hex(__uint128_t n) {
    char buffer[33];
    buffer[32] = '\0';
    for (int i = 31; i >= 0; i--) {
        uint8_t digit = n & 0xF;  // Extract lowest 4 bits.
        if (digit < 10)
            buffer[i] = '0' + digit;
        else
            buffer[i] = 'a' + (digit - 10);
        n >>= 4;
    }
    printf("%s", buffer);
}

// Improved string-to-integer conversion with better collision resistance
__uint128_t string_to_uint128(const char *s) {
    __uint128_t x = 0;
    __uint128_t base = 257;  // Larger prime base for better distribution
    size_t length = 0;
    
    while (*s) {
        x = x * base + (unsigned char)(*s);
        s++;
        length++;
    }
    
    // Mix in the length to prevent extension attacks
    return x ^ ((__uint128_t)length << 64) ^ length;
}

// Rotate bits left in a 128-bit value
__uint128_t rotl128(__uint128_t x, int r) {
    return (x << r) | (x >> (128 - r));
}

// Improved 128-bit hash function with better diffusion and avalanche effect
__uint128_t hash_function_128(__uint128_t x) {
    // Multiple seed constants with good distribution properties
    const __uint128_t K1 = ((__uint128_t)0x9e3779b97f4a7c15ULL << 64) | 0x2b5e572ad19ac784ULL;
    const __uint128_t K2 = ((__uint128_t)0xd1b54a32d192ed03ULL << 64) | 0x8cb92ba72f3d8dd7ULL;
    const __uint128_t K3 = ((__uint128_t)0xf1bbcdc8ac6f94fdULL << 64) | 0x4c8e0b3b5e606d65ULL;
    
    // Multiple rounds of mixing with different operations
    // Round 1
    x *= K1;
    x = rotl128(x, 13);
    x *= K2;
    
    // Round 2 - mix high and low bits
    __uint128_t high = x >> 64;
    __uint128_t low = x & 0xFFFFFFFFFFFFFFFFULL;
    x = (high ^ low) | (low << 64);
    
    // Round 3
    x = rotl128(x, 29);
    x *= K3;
    x ^= x >> 37;
    
    // Finalization - additional diffusion
    x ^= x >> 67;
    x *= K2;
    x ^= x >> 41;
    
    return x;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s hash <input>\n", argv[0]);
        return 1;
    }
    
    // Expect the first argument to be "hash"
    if (strcmp(argv[1], "hash") != 0) {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }
    
    char *input = argv[2];
    
    // Step 1: Convert the input string to a 128-bit number.
    __uint128_t x = string_to_uint128(input);
    
    // Step 2: Compute the 128-bit hash.
    __uint128_t hash = hash_function_128(x);
    
    // Step 3: Output the hash as 32 hexadecimal digits.
    printf("Input: %s\n", input);
    printf("128-bit hash (32 hex digits): ");
    print_uint128_hex(hash);
    printf("\n");
    
    return 0;
}
