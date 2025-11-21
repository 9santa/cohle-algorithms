#include <bits/stdc++.h>

using namespace std;


// Set of elements S is independent if and only if {ground_set[i]: i \in S} is independent over GF2
// basis will act as the internal state, pass any vector
template<class T>
auto make_GF2_linear_matroid(const vector<T> &ground_set, vector<T> &basis){
	for(auto x: ground_set) assert(x >= 0);
	auto check = [&](int i)->bool{
		T x = ground_set[i];
		for(auto y: basis) x = min(x, x ^ y);
		return x;
	};
	auto insert = [&](int i)->void{
		T x = ground_set[i];
		for(auto y: basis) x = min(x, x ^ y);
		basis.insert(lower_bound(basis.begin(), basis.end(), x, greater{}), x);
	};
	auto clear = [&]()->void{
		basis.clear();
	};
	return tuple{check, insert, clear};
}
