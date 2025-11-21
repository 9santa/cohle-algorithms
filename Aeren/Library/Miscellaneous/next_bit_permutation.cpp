#include <bits/stdc++.h>

using namespace std;


template<class T>
T next_bit_permutation(T x){
	T y = x | x - 1;
	if constexpr(sizeof(T) == 32) return y + 1 | (~y & -~y) - 1 >> (__builtin_ctz(x) + 1);
	else return y + 1 | (~y & -~y) - 1 >> (__builtin_ctzll(x) + 1);
}
