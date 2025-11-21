#include <bits/stdc++.h>

using namespace std;


template<class T>
T jacobi_symbol(T a, T n){
	assert(n > 0 && ~n & 1);
	a = a % n;
	T t = 1, r;
	for(; a; a %= n){
		while(a % 2 == 0){
			a /= 2;
			r = n % 8;
			if(r == 3 || r == 5) t = -t;
		}
		r = n, n = a, a = r;
		if(a % 4 == 3 && n % 4 == 3) t = -t;
	}
	return n == 1 ? t : 0;
}
