#include <bits/stdc++.h>
#include <cstdint>
#include <ctime>

#include "../../../it/libs/large_number.h"


using namespace std;


large_number fib(uint64_t n) {
    large_number f0(1);
    large_number f1(1);

    do {
        large_number f2 = f1;
        f1 += f0;
        f0 = f2;
        n--;
    } while (n > 2);

    return f1;
}


int main(int argc, char *argv[])
{
    clock_t start = clock();
    ios::sync_with_stdio(0);
    cin.tie(0);

    // 50000th fibonacci number in 0.5 sec
    cout << fib(50000) << "\n";


    double run_time = (double)(clock() - start) / CLOCKS_PER_SEC;
	cerr << "\nRun Time : " << std::fixed << std::setprecision(6) << run_time << " seconds\n";

    return 0;
}
