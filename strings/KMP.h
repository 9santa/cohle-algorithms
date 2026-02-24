#include "prefix_function.h"

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
            j = pi[m-1];
        }
    }
    return res;
}
