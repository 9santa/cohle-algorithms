#include <bits/stdc++.h>

using namespace std;


// Returns three non-negative integers a, b, c with n=a(a+1)/2+b(b+1)/2+c(c+1)/2
// T must be able to hold the square of n
// Runs in expected polynomial time assuming the extended Riemann hypothesis
// Requires solve_fermat_two_square and solve_legendre_three_square
template<class T>
array<T, 3> solve_fermat_three_triangle(T n){
	assert(n >= 0);
	auto res = solve_legendre_three_square(8 * n + 3);
	res = {res[0] / 2, res[1] / 2, res[2] / 2};
	assert(*min_element(res.begin(), res.end()) >= 0);
	assert(res[0] * (res[0] + 1) / 2 + res[1] * (res[1] + 1) / 2 + res[2] * (res[2] + 1) / 2 == n);
	return res;
}
