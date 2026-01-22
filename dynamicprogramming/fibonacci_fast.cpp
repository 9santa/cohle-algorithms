#include <bits/stdc++.h>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <stdexcept>


using namespace std;

constexpr uint64_t MAX = 93;

uint64_t fib(uint64_t n) {
    static uint64_t f1 = 1, f2 = 1;

    if(n <= 2) {
        return f2;
    } if (n >= MAX) {
        throw std::invalid_argument("Cannot compute for n>=" + std::to_string(MAX) +
                                    " due to limit of 64-bit integers");
    }

    uint64_t temp = f2;
    f2 += f1;
    f1 = temp;

    return f2;
}

int main(void)
{
    clock_t start = clock();
    ios::sync_with_stdio(0);
    cin.tie(0);

    // runs in 0.000024 sec
    for(int i = 1; i < 93; i++) {
        cout << fib(i) << "\n";
    }

    
    double run_time = (double)(clock() - start) / CLOCKS_PER_SEC;
	cerr << "\nRun Time : " << std::fixed << std::setprecision(6) << run_time << " seconds\n";

    return 0;
}
