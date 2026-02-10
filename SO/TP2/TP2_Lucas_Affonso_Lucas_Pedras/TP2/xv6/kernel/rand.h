#ifndef _RAND_H_
#define _RAND_H_

#include "types.h"

#define RAND_MAX_SIGNED     0x7FFFFFFF  // Equivalent to INT_MAX
#define RAND_MAX_UNSIGNED   0xFFFFFFFFU // Equivalent to UINT_MAX

// Xorshift Pseudo Random Number Generator.
// Returns a number between 0 and RAND_MAX_UNSIGNED, both inclusive.
// Adapted from: https://www.jstatsoft.org/article/view/v008i14
uint32 rand() {
    static uint32 x = 123456789U, y = 362436069U, z = 521288629U, w = 88675123U; // Initial seeds

    uint32 t;
    t = (x ^ (x << 11));
    x = y;
    y = z;
    z = w; 
    return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

// Ranged Pseudo Random Number Generator.
// Returns a number between min and max, both inclusive.
// Number returned will be <= than RAND_MAX_SIGNED.
// Adapted from: https://stackoverflow.com/questions/2509679/how-to-generate-a-random-integer-number-from-within-a-range
int rand_range(int min, int max) {
    // max <= RAND_MAX_SIGNED < RAND_MAX_UNSIGNED, so this is okay.
    uint32 num_bins = (uint32) max + 1 - min;
    uint32 num_rand = (uint32) RAND_MAX_SIGNED + 1;
    uint32 bin_size = num_rand / num_bins;
    uint32 defect   = num_rand % num_bins;

    int x;
    do {
        x = (int) rand();
    }

    // This is carefully written not to overflow
    while (num_rand - defect <= (uint32) x);

    // Truncated division is intentional
    return x / bin_size + min;
}

#endif // _RAND_H_