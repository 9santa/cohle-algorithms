#include <bits/stdc++.h>

using namespace std;


// binary exponentiation on monoid
// O(log e) applications of multiply
template<class T, class U, class F = multiplies<>>
T binary_power_general(T b, U e, F multiply = multiplies<>(), T one = 1){
	T res = one;
	for(; e; e >>= 1, b = multiply(b, b)) if(e & 1) res = multiply(res, b);
	return res;
}
