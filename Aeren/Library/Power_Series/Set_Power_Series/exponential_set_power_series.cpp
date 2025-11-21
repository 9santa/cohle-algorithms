#include <bits/stdc++.h>

using namespace std;


// Implementation of https://codeforces.com/blog/entry/92183
// O(n * log(n)^2)
// Requires convolve_subset
template<class T, int SZ = 20>
vector<T> exponential_set_power_series(const vector<T> &p){
	int n = (int)p.size();
	assert(__builtin_popcount(n) == 1 && p[0] == 0);
	int w = __lg(n);
	vector<T> res{1};
	for(auto bit = 0; bit < w; ++ bit){
		auto shift = convolve_subset<T, SZ>(res, vector<T>(p.begin() + (1 << bit), p.begin() + (1 << bit + 1)));
		res.insert(res.end(), shift.begin(), shift.end());
	}
	return res;
}
