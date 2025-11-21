#include <bits/stdc++.h>

using namespace std;


// Calculate the determinant of the sparse matrix
// Reads dimension n and its elements in form tuple{row, column, value}
// T must be of modular type
// O(n * (n + |a|) + |a| * log(mod))
// Requires linear_recurrence_solver, and sparse_minimal_polynomial
template<class T> // calculate the determinant of the sparse matrix
T sparse_determinant(auto &&rng, int n, vector<tuple<int, int, T>> a){
	vector<T> D(n);
	T det_D = 1;
	for(auto &x: D){
		do x = rng(); while(!x);
		det_D *= x;
	}
	for(auto &[i, j, val]: a) val *= D[i];
	T det = sparse_minimal_polynomial(rng, n, a)[0] * (n & 1 ? -1 : 1);
	return det / det_D;
}
