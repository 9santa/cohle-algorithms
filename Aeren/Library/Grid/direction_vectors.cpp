#include <bits/stdc++.h>

using namespace std;


namespace direction_vectors{
	vector<array<int, 2>> dr2{{1, 0}, {0, 1}};
	vector<array<int, 2>> dr4{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
	vector<array<int, 2>> dr4diag{{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
	vector<array<int, 2>> dr8{{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
	vector<array<int, 2>> drk{{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};
	vector<array<int, 2>> generate(int low, int high){
		assert(0 <= low && low <= high);
		int th = sqrt(high) + 1;
		vector<array<int, 2>> dr;
		for(auto x = -th; x <= th; ++ x) for(auto y = -th; y <= th; ++ y) if(auto d = x * x + y * y; low <= d && d <= high) dr.push_back({x, y});
		return dr;
	}
}
