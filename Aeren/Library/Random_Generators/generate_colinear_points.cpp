#include <bits/stdc++.h>

using namespace std;


// Requires points
vector<point<int>> generate_colinear_points(auto &&rng, int n, int low, int high){
	vector<point<int>> a(n);
	while(true){
		for(auto &[x, y]: a) x = randint(low, high + 1)(rng), y = randint(low, high + 1)(rng);
		for(auto i = 0; i < n; ++ i) for(auto j = i + 1; j < n; ++ j)for(auto k = j + 1; k < n; ++ k) if(doubled_signed_area(a[i], a[j], a[k]) == 0) goto NEXT;
		break;
		NEXT:;
	}
	return a;
}
