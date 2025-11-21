#include <bits/stdc++.h>

using namespace std;


// Let f[i] = 0^i + 2^i + ... + (len-1)^i
// Returns f[0], f[1], ..., f[n-1]
// O(polymul(n))
// Requires modular, number_theoric_transform, and power_series
template<class T, class FFT>
vector<T> power_sum_prefix_evaluation(long long len, int n){
	assert(n >= 0);
	if(!n) return {};
	if(!len) return vector<T>(n, T());
	auto res = ((1 - power_series_base<T, FFT>::EGF(n + 2, len)).drop(1) * (1 - power_series_base<T, FFT>::EGF(n + 2)).drop(1).inverse(n + 1)).EGF_to_seq();
	res.resize(n + 1);
	return res;
}
