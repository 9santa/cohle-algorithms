#include <bits/stdc++.h>

using namespace std;


template<class T>
string_view type_name(){
	const auto s = string_view(__PRETTY_FUNCTION__);
	return s.substr(39, s.size() - 89);
}
