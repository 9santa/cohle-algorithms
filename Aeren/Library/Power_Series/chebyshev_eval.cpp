#include <bits/stdc++.h>

using namespace std;


// O(log(n))
template<class T, class U>
T chebyshev_eval(U n, T x){
	if(n < 0) n = -n;
	auto recurse = [&](auto self, U n, T x)->array<T, 2>{
		if(n == 0) return {1, x};
		auto [a, b] = self(self, n >> 1, x);
		auto c = 2 * a * a - 1, d = 2 * a * b - x;
		if(~n & 1) return {c, d};
		auto e = 2 * x * d - c;
		return {d, e};
	};
	return recurse(recurse, n, x)[0];
}
