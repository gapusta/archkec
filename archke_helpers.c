#include <stdio.h>

#define CHAR_BIT  __CHAR_BIT__

/*
 Prints the binary representation of an unsigned long.
 The output is the most significant bit first.
 */
void printUulBits(unsigned long long n) {
    // Determine the number of bits in an unsigned long.
    // sizeof(n) gives bytes, CHAR_BIT gives bits per byte.
    const int num_bits = sizeof(n) * CHAR_BIT;

    // Start with a mask where only the most significant bit is set.
    // The cast to unsigned long is important to prevent issues on 32-bit systems
    // where 1 << 31 could be problematic for signed ints.
    unsigned long long mask = 1UL << (num_bits - 1);

    for (int i = 0; i < num_bits; i++) {
        // Use bitwise AND to check if the current bit is set.
        if (n & mask) {
            printf("1");
        } else {
            printf("0");
        }
        // Right shift the mask to check the next bit in the next iteration.
        mask >>= 1;
    }
}