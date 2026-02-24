#include "../header.h"

vector<int> z_function(const string& s) {
    int n = sz(s);
    vi z(n);
    int l = 0, r = 0;
    for (int i = 1; i < n; i++) {
        if (i <= r) z[i] = min(r-i+1, z[i-l]);
        while (i + z[i] < n && s[i+z[i]] == s[z[i]]) {
            z[i]++;
        }
        if (i + z[i] - 1 > r) {
            l = i;
            r = i + z[i] - 1;
        }
    }
    return z;
}

// Search the substring (find occurences of some pattern in text)
// O(|s| + |p|)
void search_substring(string s, const string& p) {
    int t = sz(s);
    s = p + '#' + s;
    vi z = z_function(s);
    int cnt = 0;
    vi matches;
    for (int i = 0; i < t; i++) {
        int k = z[i+sz(p)+1];
        if (k == sz(p)) {
            cnt++;
            matches.pb(i);
        }
    }
}

// Same thing just different indexing
void search_substring_alt(string s, const string& p) {
    s = p + '#' + s;
    vi z = z_function(s);
    int cnt = 0;
    vi matches;
    for (int i = sz(p)+1; i < sz(s); i++) {
        if (z[i] == sz(p)) {
            cnt++;
            matches.pb(i - sz(p) - 1);
        }
    }
}
