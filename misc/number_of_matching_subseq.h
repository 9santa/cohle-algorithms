
// Binary search, O(|s| + sum(|w|) * log |S|)
int numMatchingSubseq(string s, vector<string>& words) {
    vector<vector<int>> pos(26);
    for (int i = 0; i < (int)s.size(); i++) {
        pos[s[i] - 'a'].push_back(i);
    }

    int ans = 0;
    for (const auto& w : words) {
        int j = -1;
        bool found = true;
        for (const char& c : w) {
            auto it = upper_bound(pos[c-'a'].begin(), pos[c-'a'].end(), j);
            if (it == pos[c-'a'].end()) { found = false; break; }
            else j = *it;
        }
        if (found) ans++;
    }
    return ans;
}

// Waiting buckets (streaming subsequnce check), O(|S| + sum(|w|))
int numMatchingSubseq(string s, vector<string>& words) {
    vector<vector<pair<const string*, int>>> buckets(26);
    for (const auto& w : words) {
        buckets[w[0] - 'a'].push_back({&w, 0});
    }
    int ans = 0;
    for (char c : s) {
        int idx = c - 'a';
        auto cur = std::move(buckets[idx]);
        buckets[idx].clear();
        for (auto& st : cur) {
            const string* w = st.first;
            int pos = st.second + 1;
            if (pos == (int)w->size()) ans++;
            else buckets[(*w)[pos] - 'a'].push_back({w, pos});
        }
    }
    return ans;
}

