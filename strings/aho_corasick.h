#include "../header.h"

template<int SIG = 26>
struct AhoCorasick {
    static constexpr char OFF = 'a';

    struct Node {
        int nxt[SIG];   // during build becomes 'go' transitions
        int fail;       // failure link
        int out_link;   // next node on failure chain that is terminal (or 0)
        int parent;     // trie parent node
        int pch;        // edge char from parent to get to this node
        vi out;         // pattern ids that end exactly at this node

        Node(int p = -1, int ch = -1) : fail(0), out_link(0), parent(p), pch(ch) {
            memset(nxt, -1, sizeof(nxt));
        }
    };

    vector<Node> t;
    vi bfs_order;       // useful for counting propagation
    vi pat_end_node;    // pat_end_node[id] = node where pattern ends

    AhoCorasick() : t(1) {} // node 0 = root

    int add_pattern(const string& s, int id) {
        if (sz(pat_end_node) <= id) pat_end_node.resize(id+1, -1);
        int v = 0;
        for (char ch : s) {
            int c = ch - OFF;
            if (t[v].nxt[c] == -1) {
                t[v].nxt[c] = sz(t);
                t.emplace_back(v, c); // set parent and pch here
            }
            v = t[v].nxt[c];
        }
        t[v].out.push_back(id);
        pat_end_node[id] = v;
        return v;
    }

    // Build failure links + complete automation transitions (go function)
    // After build: t[v].nxt[c] is always defined (0..nodes-1)
    void build() {
        queue<int> q;
        bfs_order.clear();
        bfs_order.reserve(sz(t));

        // init root transitions: missing -> 0, real children -> fail=0
        for (int c = 0; c < SIG; c++) {
            int u = t[0].nxt[c];
            if (u == -1) {
                t[0].nxt[c] = 0; // missing from root loops to root
            } else {
                t[u].fail = 0;
                t[u].out_link = 0; // will be set properly below, but root has none
                q.push(u);
            }
        }

        bfs_order.push_back(0);

        while (!q.empty()) {
            int v = q.front(); q.pop();
            bfs_order.push_back(v);

            // compute out_link: jump to nearest terminal node on fail-chain
            int f = t[v].fail;
            t[v].out_link = (!t[f].out.empty() ? f : t[f].out_link);

            for (int c = 0; c < SIG; c++) {
                int u = t[v].nxt[c];
                if (u != -1) {
                    // set failure link for child
                    t[u].fail = t[t[v].fail].nxt[c];
                    q.push(u);
                } else {
                    // complete automation transition
                    t[v].nxt[c] = t[t[v].fail].nxt[c];
                }
            }
        }
    }

    // Report matches: calls callback(pos_end, pattern_id)
    template<class F>
    void match(const string& text, F on_match) const {
        int v = 0;
        for (int i = 0; i < sz(text); i++) {
            int c = text[i] - OFF;
            v = t[v].nxt[c];

            // patterns ending at v
            for (int id : t[v].out) on_match(i, id);

            // patterns ending on terminal nodes up the out_link chain
            int u = t[v].out_link;
            while (u != 0) {
                for (int id : t[u].out) on_match(i, id);
                u = t[u].out_link;
            }
        }
    }

    // Count occurrences of each pattern id (patterns are added with ids 0..P-1)
    vl count_occurrences(const string& text, int P) const {
        vl occ_node(t.size(), 0);

        // walk text and count visits to states
        int v = 0;
        for (char ch : text) {
            int c = ch - OFF;
            v = t[v].nxt[c];
            occ_node[v]++;
        }

        // propagate counts upward on failure tree (rev BFS order)
        // If state v is visited X times, then all suffix states also occur X times
        for (int i = sz(bfs_order)-1; i >= 1; i--) {
            int x = bfs_order[i];
            occ_node[t[x].fail] += occ_node[x];
        }

        vl occ_pat(P, 0);
        for (int id = 0; id < P; id++) {
            int node = (id < sz(pat_end_node) ? pat_end_node[id] : -1);
            if (node != -1) occ_pat[id] = occ_node[node];
        }
        return occ_pat;
    }
};

void example() {
    AhoCorasick<26> ac;
    ac.add_pattern("he", 0);
    ac.add_pattern("she", 1);
    ac.add_pattern("his", 2);
    ac.add_pattern("hers", 3);
    ac.build();

    string text = "ahishers";
    ac.match(text, [&](int pos, int id) {
        cout << "match id=" << id << " ends at " << pos << "\n";
    });

    auto occ = ac.count_occurrences(text, 4);
}
