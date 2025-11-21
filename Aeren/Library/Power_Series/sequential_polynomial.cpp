#include <bits/stdc++.h>

using namespace std;


// Returns (x - a) * (x - (a + 1)) * ... * (x - (a + n - 1))
// O(n * log(n))
// Requires modular, number_theoric_transform, and taylor_shift
template<class T, class FFT = ntt>
vector<T> sequential_polynomial(int n, T a){
	assert(n >= 0);
	if(n == 0) return {1};
	if(n == 1) return {-a, 1};
	vector<T> res{1}, base{-a, 1};
	for(auto bit = 0, lg = __lg(n); bit <= lg; ++ bit){
		if(n & 1 << bit) res = FFT::convolve(base, taylor_shift<T, FFT>(res, 1 << bit));
		base = FFT::convolve(base, taylor_shift<T, FFT>(base, 1 << bit));
	}
	return res;
}
