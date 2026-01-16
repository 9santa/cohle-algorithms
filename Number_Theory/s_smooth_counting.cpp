
// Count how many numbers in the range [l, r] are good.
// A good number has only 2-digit primes in its factorization.

const vector<ll> primes = {11,13,17,19,23,29,31,37,41,43,
                    47,53,59,61,67,71,73,79,83,89,97};
constexpr ll LIMIT = 1e18L;


void generate(vector<int>const& p, int idx, ll curr, vector<ll>& out) {
    if(idx == (int)primes.size()) {  // if this is the last prime
        out.push_back(curr);
        return;
    }

    ll val = curr;
    while(val <= LIMIT) {
        generate(p, idx+1, val, out);
        if (val > LIMIT / p[idx]) break;
        val *= p[idx];
    }
}

ll countGood(ll N, vector<ll>const& left, vector<ll>const& right) {
    ll ans = 0;
    for (auto a : left) {
        if (a > N) continue;
        ll limit = N/a;
        ans += upper_bound(right.begin(), right.end(), limit) - right.begin();
    }

    return ans-1;
}

signed main(void) {
    ios::sync_with_stdio(0);
    cin.tie(0);

    vector<int> p1(primes.begin(), primes.begin() + primes.size()/2);
    vector<int> p2(primes.begin() + primes.size()/2, primes.end());

    vector<ll> A, B;
    generate(p1, 0, 1, A);

    generate(p2, 0, 1, B);
    sort(A.begin(), A.end());
    sort(B.begin(), B.end());

    ll L, R; cin >> L >> R;
    ll ans = countGood(R, A, B) - countGood(L-1, A, B);


    return 0;
}
