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
vector<int> KMP(string s, const string& p){
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
    return matches;
}

vector<int> kmp_mem_optimized(const string& text, const string& p) {
    vector<int> res;
    if (p.empty()) return res;

    auto pi = prefix_function(p);
    int n = sz(text);
    int m = sz(p);

    int j = 0;
    for (int i = 0; i < n; i++) {
        while (j > 0 && text[i] != p[j]) {
            j = pi[j-1];
        }
        if (text[i] == p[j]) {
            j++;
        }
        if (j == m) {
            res.push_back(i - m + 1); // starting index of the match
            j = pi[j-1];
        }
    }
    return res;
}
