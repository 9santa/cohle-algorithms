#pragma once
#include <bits/stdc++.h>
using namespace std;

#define sz(x) int((x).size())

/** Returns whether S is a subsequence of T. Time: O(|S| + |T|). */
template<typename STRING>
bool is_subseq(const STRING& S, const STRING& T) {
    int p = 0;
    for (const auto&& c : S) {
        while (p < sz(S) && T[p] != c) p++;
        if (p == sz(T)) return false;
        p++;
    }
    return true;
}
