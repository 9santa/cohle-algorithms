#include "../header.h"

// Computes the prefix function (pi array) for string s.
// pi[i] = the length of the longest proper prefix of s[0..i]
//         which is also a suffix of s[0..i].
// O(|s|)
vector<int> prefix_function(const string& s) {
    int n = sz(s);
    vi pi(n);
    for (int i = 1; i < n; i++) {
        int j = pi[i-1];
        while (s[i] != s[j] && j != 0) j = pi[j-1];
        if (s[i] == s[j]) j++;
        pi[i] = j;
    }
    return pi;
}

// Knuth-Morris-Prath
void KMP(string s, const string& p){
    s = p + '#' + s;
    vi pi = prefix_function(s);
    int cnt = 0;
    vi matches;
    for (int i = 0; i < sz(s); i++) {
        if (pi[i] == sz(p)) {
            cnt++;
            matches.pb(i - 2*(sz(p)));
        }
    }
}
