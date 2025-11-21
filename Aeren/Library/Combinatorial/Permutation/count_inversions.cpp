#include <bits/stdc++.h>

using namespace std;


// O(n * log(n))
template<class T, class Compare = less<>>
long long count_inversions(const vector<T> &a, bool count_equal = false, Compare cmp = less<>()){
	int n = (int)a.size();
	vector<T> cmpr = a;
	sort(cmpr.begin(), cmpr.end(), cmp);
	vector<int> sum(n);
	long long res = 0;
	for(auto i = 0; i < n; ++ i){
		int p = lower_bound(cmpr.begin(), cmpr.end(), a[i], cmp) - cmpr.begin();
		res += i;
		for(auto r = p + !count_equal; r > 0; r -= r & -r) res -= sum[r - 1];
		for(++ p; p <= n; p += p & -p) ++ sum[p - 1];
	}
	return res;
}
