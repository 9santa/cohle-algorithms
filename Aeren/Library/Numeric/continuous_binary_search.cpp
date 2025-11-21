#include <bits/stdc++.h>

using namespace std;


// pred(x) is [T...T][F...F] for [low, high]
// Returns the point where pred changes
// O(iter)
template<class T, int iter = 100>
T continuous_binary_search(T low, T high, auto pred){
	static_assert(is_same_v<T, double> || is_same_v<T, long double>);
	assert(low <= high);
	for(auto rep = 0; rep < iter; ++ rep){
		T mid = (low + high) / 2;
		pred(mid) ? low = mid : high = mid;
	}
	return (low + high) / 2;
}
