#include <bits/stdc++.h>

using namespace std;


// Assumes the convex polygon contains at least one point
// Requires point and convex_polygon
// O(n)
template<class T>
array<int, 2> convex_polygon_diameter(const convex_polygon<T> &c){
	assert(!c.empty());
#define NEXT(i) i < (int)c.size() - 1 ? i + 1 : 0
	if((int)c.size() == 1) return {0, 0};
	pair<T, array<int, 2>> res{0, {0, 0}};
	for(auto i = 0, j = 1; i < (int)c.size(); ++ i){
		while(true){
			res = max(res, decltype(res){squared_distance(c[i], c[j]), {i, j}});
			if((c[NEXT(i)] - c[i] ^ c[NEXT(j)] - c[j]) <= 0) break;
			j = NEXT(j);
		}
	}
#undef NEXT
	return res.second;
}
