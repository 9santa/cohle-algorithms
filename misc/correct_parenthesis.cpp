#include <bits/stdc++.h>

using namespace std;

bool is_valid(const string& s) {
    stack<char> st;
    unordered_map<char, char> brackets = {
        {')', '('},
        {']', '['},
        {'}', '{'}
    };

    for(char c : s) {
        if(c == '(' || c == '[' || c == '{') {
            st.push(c);
        } else if (c == ')' || c == ']' || c == '}') {
            if(st.empty() || st.top() != brackets[c]) {
                return false;
            }
            st.pop();
        }
    }

    return st.empty();
}


signed main(void) {
    ios::sync_with_stdio(0);
    cin.tie(0);

    string s; cin >> s;
    cout << is_valid(s);


    return 0;
}
