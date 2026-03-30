#pragma once
#include "../../graphs/core.h"

/* you need the answer for every node as root.
   the idea is:
    1. compute child-to-parent values
    2. compute parent-to-child values using prefix/suffix products
   answer for child = everything from parent side except that child */

// https://atcoder.jp/contests/dp/tasks/dp_v

// combine(a, b): merges contributions
// addRoot(x, u): turns merged children contributions into answer for node u

vector<vector<int>> g;
vector<ll> down; // down[u] = contribution of the subtree of u to its parent
vector<ll> up;   // up[u] = contribution coming into u from the parent side
vector<ll> ans;  // ans[u] = final ans for node u using all directions

ll combine(ll a, ll b) {
    // replace with problem-specific merge
    return a * b;
}

ll addRoot(ll x, int u) {
    // replace with problem-specific "attach node u"
    return x + 1;
}

// contributions coming from children
void dfs1(int u, int p) {
    ll cur = 1;
    for (auto v : g[u]) {
        if (v == p) continue;
        dfs1(v, u);
        cur = combine(cur, down[v]);
    }
    down[u] = addRoot(cur, u);
}

void dfs2(int u, int p) {
    int m = sz(g[u]);
    vl pref(m+1, 1), suf(m+1, 1);

    /*  At node u, suppose neighbor contribs are: c[0], c[1], ..., c[m-1]
        For child v = g[u][i] we need: combine of all c[j] where j != i.
        pref and suf are for O(1) child transitions:
        contribution of "all except i" is: combine(pref[i], suf[i+1]) */
    for (int i = 0; i < m; i++) {
        int v = g[u][i];
        ll val = (v == p ? up[u] : down[v]);
        pref[i+1] = combine(pref[i], val);
    }
    for (int i = m-1; i >= 0; i--) {
        int v = g[u][i];
        ll val = (v == p ? up[u] : down[v]);
        suf[i] = combine(suf[i+1], val);
    }

    // pref[m] is the combination of all neighbor contributions
    // so attaching u gives the full ans for u
    ans[u] = addRoot(pref[m], u);

    for (int i = 0; i < m; i++) {
        int v = g[u][i];
        if (v == p) continue;
        ll parentSideForV = combine(pref[i], suf[i+1]); // combine all contribs around u except the one coming from v's subtree
        up[v] = addRoot(parentSideForV, u);
        dfs2(v, u);
    }
}


// More generic version
template <class T>
struct Rerooting {
    int n;
    vector<vector<int>> g;
    vector<T> down, up, ans;
    T ID;

    function<T(T,T)> combine;
    function<T(T,int)> addRoot;

    Rerooting(int n, T id,
              function<T(T,T)> combine,
              function<T(T,int)> addRoot)
        : n(n), g(n), down(n), up(n), ans(n),
          ID(id), combine(combine), addRoot(addRoot) {}

    void addEdge(int u, int v) {
        g[u].push_back(v);
        g[v].push_back(u);
    }

    void dfs1(int u, int p) {
        T cur = ID;
        for (int v : g[u]) {
            if (v == p) continue;
            dfs1(v, u);
            cur = combine(cur, down[v]);
        }
        down[u] = addRoot(cur, u);
    }

    void dfs2(int u, int p) {
        int m = g[u].size();
        vector<T> pref(m + 1, ID), suff(m + 1, ID);

        for (int i = 0; i < m; i++) {
            int v = g[u][i];
            T val = (v == p ? up[u] : down[v]);
            pref[i + 1] = combine(pref[i], val);
        }
        for (int i = m - 1; i >= 0; i--) {
            int v = g[u][i];
            T val = (v == p ? up[u] : down[v]);
            suff[i] = combine(suff[i + 1], val);
        }

        ans[u] = addRoot(pref[m], u);

        for (int i = 0; i < m; i++) {
            int v = g[u][i];
            if (v == p) continue;
            T withoutV = combine(pref[i], suff[i + 1]);
            up[v] = addRoot(withoutV, u);
            dfs2(v, u);
        }
    }

    vector<T> solve(int root = 0) {
        dfs1(root, -1);
        up[root] = ID;
        dfs2(root, -1);
        return ans;
    }
};
