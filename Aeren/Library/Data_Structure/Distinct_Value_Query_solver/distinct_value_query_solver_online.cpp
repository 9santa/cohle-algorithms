#include <bits/stdc++.h>

using namespace std;


struct distinct_value_query_solver_online{
	int n;
	// 0 <= a[i] < sigma must hold for all 0 <= i < n
	// build(a): prepare a static data structure to process the following queries: given l, r, and k, count the number of elements in a[ql, qr) less than k
	// O(n * log(sigma) + sigma)
	distinct_value_query_solver_online(const vector<int> &a, int sigma, auto build): n((int)a.size()){
		for(auto &x: a) assert(0 <= x && x < sigma);
		vector<int> seen(sigma), aux(n);
		for(auto i = 0; i < n; ++ i){
			aux[i] = seen[a[i]];
			seen[a[i]] = i + 1;
		}
		build(aux);
	}
	// 0 <= a[i] < sigma must hold for all 0 <= i < n
	// build(a, w): prepare a static data structure to process the following queries: given ql, qr, and k, find the aggregate of w[i] over all indices i with ql <= i < qr and a[i] < k
	// O(n * log(sigma) + sigma)
	template<class T>
	distinct_value_query_solver_online(const vector<int> &a, const vector<T> &w, auto build): n((int)a.size()){
		int sigma = (int)w.size();
		for(auto &x: a) assert(0 <= x && x < sigma);
		vector<int> seen(sigma), aux(n);
		vector<T> aux_w(n);
		for(auto i = 0; i < n; ++ i){
			aux[i] = seen[a[i]];
			aux_w[i] = w[a[i]];
			seen[a[i]] = i + 1;
		}
		build(aux, w);
	}
	// Find the sum of w[x] over all distinct values x appearing in a[ql, qr)
	// _query(ql, qr, k): returns the answer to the query defined by (ql, qr, k)
	// O(log(n))
	auto query(int ql, int qr, auto _query) const{
		assert(0 <= ql && ql <= qr && qr <= n);
		return _query(ql, qr, ql + 1);
	}
};
