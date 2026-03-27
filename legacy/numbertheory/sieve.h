
constexpr int MAXN = 100;
static u8 isPrime[MAXN+1];
vector<int> primes;

// O(N log log N)
void sieve(void) {
    memset(isPrime, true, sizeof(isPrime));
    isPrime[0] = isPrime[1] = 0;    // 0 and 1 are not primes

    for (int i = 2; 1LL * i * i <= MAXN; i++) {
        if (isPrime[i]) {
            primes.push_back(i);
            for (ll j = 1LL * i * i; j <= MAXN; j += i) {
                 isPrime[(int)j] = 0;
            }
        }
    }

    primes.reserve(1e5);
    for (int i = 2; i <= MAXN; i++) {
        if (isPrime[i]) primes.push_back(i);
    }

}

// Segmented Sieve. Less memory + cache friendly. Counts number of primes up to N
int count_primes(int n) {
    const int S = 10000; // block size
    vector<int> primes;
    int nsqrt = sqrt(n);
    vector<char> is_prime(nsqrt+2, true);

    for (int i = 2; i <= nsqrt; i++) {
        if (is_prime[i]) {
            primes.push_back(i);
            for (int j = i*i; j <= nsqrt; j += i)
                is_prime[j] = false;
        }
    }

    int res = 0;
    vector<char> block(S);

    for (int k = 0; k * S <= n; k++) {
        fill(block.begin(), block.end(), true);
        int start = k * S;

        // mark multiples of primes in the current block
        for (int p : primes) {
            int start_idx = (start + p - 1) / p;
            int j = max(start_idx, p) * p - start;
            for (; j < S; j += p)
                block[j] = false;
        }

        if (k == 0) block[0] = block[1] = false;

        // count primes in the block
        for (int i = 0; i < S && start + i <= n; i++) {
            if (block[i]) res++;
        }
    }

    return res;
}
