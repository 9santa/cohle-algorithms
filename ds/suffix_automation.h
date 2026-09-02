#pragma once
#include <bits/stdc++.h>
using namespace std;

struct SuffixAutomation {
    struct Node {
        map<char, int> next;
        int len = 0;
        int link = -1;
        bool is_term = false;
    };

    vector<Node> nodes;
    string s;
    int last; // [S]s

    SuffixAutomation(int max_len = 0) {
        nodes.reserve(2 * max_len);
        nodes.push_back(Node());
        s = "";
        last = 0;
    }

    void extend(char c) {
        s.push_back(c);
        int nlast = (int)nodes.size(); // [Sc]Sc
        nodes.push_back(Node({}, s.length(), -1, false));

        int ptr = last;
        while (ptr != -1 && !nodes[ptr].next.count(c)) {
            nodes[ptr].next[c] = nlast;
            ptr = nodes[ptr].link;
        }

        if (ptr < 0) {
            nodes[nlast].link = 0;
            last = nlast;
            return;
        }

        int q = nodes[ptr].next[c];
        if (nodes[ptr].len + 1 == nodes[q].len) {
            nodes[nlast].link = q;
            last = nlast;
            return;
        }

        int clone = (int)nodes.size();
        nodes.push_back(nodes[q]);
        nodes[clone].len = nodes[ptr].len + 1;
        nodes[clone].is_term = false;
        nodes[q].link = nodes[nlast].link = clone;

        while (ptr >= 0) {
            auto it = nodes[ptr].next.find(c);
            if (it == nodes[ptr].next.end() || it->second != q) break;
            nodes[ptr].next[c] = clone;
            ptr = nodes[ptr].link;
        }

        last = nlast;
    }

    void build(const string& s) {
        for (char c : s) extend(c);
    }

    bool contains(const string& t) const {
        int v = 0;
        for (char c : t) {
            auto it = nodes[v].next.find(c);
            if (it == nodes[v].next.end()) return false;
            v = it->second;
        }
        return true;
    }

    long long countDistinctSubstrings() const {
        long long ans = 0;
        for (int v = 1; v < (int)nodes.size(); v++) {
            ans += nodes[v].len - nodes[nodes[v].link].len;
        }
        return ans;
    }

    void mark_terminals() {
        int ptr = last;
        while (ptr >= 0) {
            nodes[ptr].is_term = true;
            ptr = nodes[ptr].link;
        }
    }
};
