#include <bits/stdc++.h>

using namespace std;


// Set of elements S is independent if and only if ground_set[i] is distinct for i \in S
// flag will act as the internal state, pass any vector
auto make_colorful_matroid(const vector<int> &ground_set, vector<int> &flag){
	for(auto x: ground_set){
		assert(0 <= x);
		if((int)flag.size() <= x) flag.resize(x + 1);
	}
	auto check = [&](int i)->bool{
		return !flag[ground_set[i]];
	};
	auto insert = [&](int i)->void{
		flag[ground_set[i]] = true;
	};
	auto clear = [&]()->void{
		fill(flag.begin(), flag.end(), false);
	};
	return tuple{check, insert, clear};
};
