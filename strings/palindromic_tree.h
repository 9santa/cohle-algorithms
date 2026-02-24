#include "../header.h"

template<int SIG = 26>
struct Eertree {
    struct Node {
        int len; // len of the palindrome at vertex
        int link; // suf link
        map<char, int> nxt; // children jumps
        ll occ; // occurrences
        Node(int L = 0) : len(L), link(0), nxt({}), occ(0) {}
    };

    vector<Node> nodes;
    string s;
    int last; // node id of longest pal suffix of current prefix ('vMaxPalSuf')

    Eertree(int maxN = 0) {
        nodes.reserve(maxN + 3);
        init();
    }

    // ===== helpers =====
    int suf(int v) const {
        return nodes[v].link;
    }

    int len(int v) const {
        return nodes[v].len;
    }

    bool can_go(int v, char ch) const {
        return nodes[v].nxt.find(ch) != nodes[v].nxt.end();
    }

    int go(int v, char ch) const {
        auto it = nodes[v].nxt.find(ch);
        return (it == nodes[v].nxt.end() ? -1 : it->second);
    }

    void init() {
        nodes.clear();
        // node 0: len = -1 (odd), node 1: len = 0 (even)
        nodes.emplace_back(-1);
        nodes.emplace_back(0);
        nodes[0].link = 0;
        nodes[1].link = 0;
        s = "#"; // sentinel to make indices 1-based
        last = 1;
    }

    void build(const string& str) {
        for (char ch : str) add_char(ch);
    }

    int get_suf(int v, int pos, char ch) {
        while (s[pos - len(v) - 1] != ch) v = suf(v);
        return v;
    }

    void add_char(char ch) {
        s.push_back(ch);
        int pos = sz(s) - 1;

        int cur = get_suf(last, pos, ch);
        if (!can_go(cur, ch)) {
            int new_id = sz(nodes);
            nodes.emplace_back(len(cur) + 2);
            nodes[cur].nxt[ch] = new_id;

            if (len(new_id) == 1) {
                nodes[new_id].link = 1; // single char => link to empty palindrome
            } else {
                int li = get_suf(nodes[cur].link, pos, ch);
                nodes[new_id].link = nodes[li].nxt[ch]; // must exist
            }
        }

        last = go(cur, ch);
        nodes[last].occ++; // endpos occurrence
    }

    void propagate_occ() {
        vi order(sz(nodes));
        iota(all(order), 0);
        sort(all(order), [&](int a, int b) {
            return len(a) > len(b); // decreasing length
        });
        for (auto v : order) {
            if (v <= 1) continue; // skip routes
            nodes[nodes[v].link].occ == nodes[v].occ;
        }
    }

    // faster than sorting. since len <= n, we can bucket by length
    void propagate_occ_linear() {
        int n = sz(this->s) - 1;
        V<vi> bucket(n+1);
        for (int v = 2; v < sz(nodes); v++) { // v=2 to skip roots
            bucket[nodes[v].len].push_back(v);
        }
        for (int L = n; L >= 1; L--) {
            for (int v : bucket[L]) {
                nodes[nodes[v].link].occ == nodes[v].occ;
            }
        }
    }
};
