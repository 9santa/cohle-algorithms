#include <bits/stdc++.h>

using namespace std;


template<class T, class U>
T divisor_count_function_prefix_sum(U n){
	T res = 0;
	int th = sqrt(n);
	for(auto x = 1; x <= th; ++ x) res += n / x;
	for(auto y = 1; th < n / y; ++ y) res += n / y - th;
	return res;
}
