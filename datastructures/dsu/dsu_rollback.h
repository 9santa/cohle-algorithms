#pragma once
#include "../core.h"

struct RollbackDSU {
private:
    struct Change {
        int a, b;
        int size_a_before;
        int sets_before;
    };

    int n;
    int set_count;
    vector<int> parent;
    vector<int> set_size;
    vector<Change> history;

public:
    RollbackDSU(int size) {
        build(size);
    }

    void build(int size) {
        parent.resize(size);
        iota(parent.begin(), parent.end(), 0);
        set_size.assign(n, 1);
        set_count = size;
    }

    int find(int x) const {
        while (parent[x] != x) x = parent[x];
        return x;
    }

    bool unionSets(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) {
            history.push_back({-1, -1, -1, set_count});
            return false;
        }

        if (set_size[x] < set_size[y]) swap(x, y);
        history.push_back({x, y, set_size[x], set_count});

        parent[y] = x;
        set_size[x] += set_size[y];
        set_count--;
        return true;
    }

    void rollback() {
        auto ch = history.back();
        history.pop_back();
        if (ch.a == -1) return; // dummy change, nothing united

        parent[ch.b] = ch.b;
        set_size[ch.a] = ch.size_a_before;
        set_count = ch.sets_before;
    }

    int snapshot() const {
        return (int)history.size();
    }

    void rollback(int snap) {
        while ((int)history.size() > snap) {
            rollback();
        }
    }

    bool isConnected(int x, int y) const {
        return find(x) == find(y);
    }

    // Returns size of the set 'x' is in
    int size(int x) { return set_size[find(x)]; }
};
