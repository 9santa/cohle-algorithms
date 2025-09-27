#include <iostream>
#include <bitset>

constexpr int MAX = 100;

void sieveOfEratosthenes(void) {
    std::bitset<MAX+1> isPrime;
    isPrime.set();  // set all bits to 1 initially

    isPrime[0] = isPrime[1] = 0;    // 0 and 1 are not primes

    for (int i = 2; i * i <= MAX; i++) {
        if (isPrime[i]) {
            for (int j = i*i; j <= MAX; j += i) {
                isPrime[j] = 0;
            }
        }
    }

    std::cout << "Prime numbers up to " << MAX << ":\n";
        for (int i = 2; i <= MAX; i++) {
            if (isPrime[i]) {
                std::cout << i << " ";
            }
        }
        std::cout << "\n";
} 
