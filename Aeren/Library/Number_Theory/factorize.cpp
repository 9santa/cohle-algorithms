#include <bits/stdc++.h>

using namespace std;


template<class T>
vector<pair<T, int>> factorize(T x){
	vector<pair<T, int>> res;
	for(T p = 2; p * p <= x; ++ p) if(x % p == 0){
		res.push_back({p, 0});
		while(x % p == 0) ++ res.back().second, x /= p;
	}
	if(x > 1) res.push_back({x, 1});
	return res;
}
