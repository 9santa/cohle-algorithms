#include <bits/stdc++.h>

using namespace std;


// Block min by default. Set cmp = greater for block max
// Returns indices
// O(n)
template<class T, class Compare = less<>>
vector<int> block_minmax_processor(const vector<T> &a, int block, Compare cmp = less<>()){
	int n = (int)a.size();
	assert(1 <= block && block <= n);
	vector<int> res(n - block + 1);
	deque<int> dq;
	for(auto i = 0; i < block - 1; ++ i){
		while(!dq.empty() && !cmp(a[dq.back()], a[i])) dq.pop_back();
		dq.push_back(i);
	}
	for(auto i = block - 1; i < n; ++ i){
		while(!dq.empty() && !cmp(a[dq.back()], a[i])) dq.pop_back();
		dq.push_back(i);
		res[i - block + 1] = dq.front();
		if(dq.front() == i - block + 1) dq.pop_front();
	}
	return res;
}
