#include <bits/stdc++.h>

using namespace std;


// Requires point and line
template<class T>
bool point_in_polygon(const point<T> &p, const vector<point<T>> &poly, bool strict = true){
	bool res = 0;
	for(auto i = 0, n = (int)poly.size(); i < n; ++ i){
		point<T> q = poly[(i + 1) % n];
		if constexpr(is_floating_point_v<T>){
			if(line<T>{poly[i], q}.distance_to_segment(p) < 1e-9) return !strict;
		}
		else if(line<T>{poly[i], q}.on_segment(p)) return !strict;
		res ^= ((p.y < poly[i].y) - (p.y < q.y)) * doubled_signed_area(p, poly[i], q) > 0;
	}
	return res;
}
