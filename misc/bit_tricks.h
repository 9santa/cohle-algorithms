#include "../header.h"
#include <bit>


/** Returns number of set bits. Time: O(1). */
int popcnt(int x) { return __builtin_popcount(x); }
int popcnt(u32 x) { return __builtin_popcount(x); }
int popcnt(ll x) { return __builtin_popcountll(x); }
int popcnt(u64 x) { return __builtin_popcountll(x); }
/** Returns (-1)^popcount(x). Time: O(1). */
int popcnt_sign(int x) { return (__builtin_parity(unsigned(x)) & 1 ? -1 : 1); }
int popcnt_sign(u32 x) { return (__builtin_parity(x) & 1 ? -1 : 1); }
int popcnt_sign(ll x) { return (__builtin_parityll(x) & 1 ? -1 : 1); }
int popcnt_sign(u64 x) { return (__builtin_parityll(x) & 1 ? -1 : 1); }
/** Returns index of highest set bit, or -1 for zero. Time: O(1). */
int topbit(int x) { return (x == 0 ? -1 : 31 - __builtin_clz(x)); }
int topbit(u32 x) { return (x == 0 ? -1 : 31 - __builtin_clz(x)); }
int topbit(ll x) { return (x == 0 ? -1 : 63 - __builtin_clzll(x)); }
int topbit(u64 x) { return (x == 0 ? -1 : 63 - __builtin_clzll(x)); }
/** Returns index of lowest set bit, or -1 for zero. Time: O(1). */
int lowbit(int x) { return (x == 0 ? -1 : __builtin_ctz(x)); }
int lowbit(u32 x) { return (x == 0 ? -1 : __builtin_ctz(x)); }
int lowbit(ll x) { return (x == 0 ? -1 : __builtin_ctzll(x)); }
int lowbit(u64 x) { return (x == 0 ? -1 : __builtin_ctzll(x)); }




// Least Significant Bit
/** Returns least significant set bit value. Time: O(1). */
int leastSB(unsigned x) {
    int lsb = x & 1;
    return lsb;
}

// Lowest Set Bit
/** Returns least significant set bit value. Time: O(1). */
int lowestSB(unsigned x) {
    int lsb = x & -x;
    return lsb;

    // another way, gives index of LSB
    int pos = std::countr_zero(x);
}

// Most Significant Bit
/** Returns highest power-of-two position by shifting. Time: O(log word). */
int bit_shift_loop(unsigned x) {
    int pos = -1;
    while (x) {
        x >>= 1;
        pos++;
    }
    return pos;
}

// using log2
int pos = std::log2(x);
int pos = __lg(x);

int pos = 31 - __builtin_clz(x);

// C++20 std::bit_width
int pos = std::bit_width(x) - 1;

// Mask Directly
unsigned msb_mask = 1u << (31 - __builtin_clz(x));

// De Bruijn trick
static const int index32[32] = {
  0, 1, 28, 2, 29, 14, 24, 3,
  30, 22, 20, 15, 25, 17, 4, 8,
  31, 27, 13, 23, 21, 19, 16, 7,
  26, 12, 18, 6, 11, 5, 10, 9
};

int msb = index32[(x * 0x077CB531U) >> 27];
