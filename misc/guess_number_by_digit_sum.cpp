#include "../header.h"

constexpr ll MAX_X = 1e18+1;

ll ask(ll a) {
    cout << "? " << a << "\n";
    cout.flush();
    ll resp;
    if (!(cin >> resp)) {
        exit(0);
    }
    return resp;
}

int sumDigits(ll a) {
    int s = 0;
    if (a < 0) a = -a;
    while (a > 0) {
        s += a % 10;
        a /= 10;
    }
    return s;
}

void solve(void) {
    constexpr int MAXD = 19;
    vector<int> digits(MAXD, 0);

    ll pow10[MAXD];
    pow10[0] = 1;
    for (int i = 1; i < MAXD; i++) {
        pow10[i] = pow10[i-1] * 10LL;
    }

    // Base query: sum(x)
    ll sumx = ask(0);

    auto check = [&](ll sum_xa, int suma) {
        return (sum_xa != sumx + suma);
    };

    // Binary search on each digit
    for (int i = 0; i < MAXD; i++) {
        int L = 1, R = 9;
        int bestd = 10;
        while (L <= R) {
            int mid = (L+R) / 2;

            ll a = pow10[i] * 1LL * mid;
            ll query = ask(a);
            int suma = sumDigits(a);

            if (check(query, suma)) {
                bestd = mid;
                R = mid - 1;
            } else {
                L = mid + 1;
            }
        }

        int xi;
        if (bestd == 10) {
            xi = 10;
        } else {
            xi = 10 - bestd;
        }
        digits[i] = xi;
    }

    // Construct value 'x'
    i128 val = 0;
    for (int i = MAXD-1; i >= 0; i--) {
        val = val * 10 + digits[i];
    }

    ll x = (ll)val;
    cout << "! " << x << "\n";
    cout.flush();
}
