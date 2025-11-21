#include <bits/stdc++.h>

using namespace std;


// O(n) applications of multiplications on T
template<class T>
T tridiagonal_matrix_determinant(const vector<T> &diag, const vector<T> &sup, const vector<T> &sub){
	int n = (int)diag.size();
	assert((int)sup.size() == n - 1 && (int)sub.size() == n - 1);
	T a = 1, b = diag[0];
	for(auto i = 1; i < n; ++ i) tie(a, b) = pair{b, diag[i] * b - sup[i - 1] * sub[i - 1] * a};
	return b;
}
