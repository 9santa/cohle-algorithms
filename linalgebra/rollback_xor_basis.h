#pragma once

#include "core.h"

/*
For offline-problems:
- add on time segments
- segment tree over time
- divide and conquer on queries
- delete via rollback
*/

/** Xor basis with rollback support for offline divide-and-conquer. Space: O(BITS + updates). */
template<int BITS = 64, typename T = u64>
struct RollbackXorBasis {
    T basis[BITS] = {0};
    ll inserted = 0;
    int rk = 0;

    struct Change {
        int pos;    // -1 if dependent insertion
        T old_value;
        ll old_inserted;
        int old_rk;
    };

    vector<Change> history;

    /** Clears the basis and rollback history. Time: O(BITS + history). */
    void clear() {
        basis = {0};
        inserted = 0;
        rk = 0;
        history.clear();
    }

    /** Returns whether the span has rank zero. Time: O(1). */
    bool empty() const { return rk == 0; }
    /** Returns current basis rank. Time: O(1). */
    int rank() const { return rk; }

    /** Returns a rollback checkpoint. Time: O(1). */
    int snapshot() const {
        return sz(history);
    }

    /** Restores a previous checkpoint. Time: O(number of reverted inserts). */
    void rollback(int snap) {
        while (sz(history) > snap) {
            auto ch = history.back();
            history.pop_back();
            inserted = ch.old_inserted;
            rk = ch.old_rk;
            if (ch.pos != -1) basis[ch.pos] = ch.old_value;
        }
    }

    /** Inserts x into the basis. Time: O(BITS). */
    bool insert(T x) {
        ll old_inserted = inserted;
        int old_rk = rk;
        inserted++;

        for (int i = BITS-1; i >= 0; i--) {
            if (((x >> i) & 1) == 0) continue;
            if (!basis[i]) {
                history.push_back({i, basis[i], old_inserted, old_rk});
                basis[i] = x;
                rk++;
                return true;
            }
            x ^= basis[i];
        }

        history.push_back({-1, 0, old_inserted, old_rk});
        return false;
    }

    /** Returns whether x is representable by the current basis. Time: O(BITS). */
    bool can(T x) const {
        for (int i = BITS-1; i >= 0; i--) {
            if (x & (1LL << i)) {
                if (!basis[i]) return false;
                x ^= basis[i];
            }
        }
        return true;
    }

    /** Maximizes seed xor y over all y in the span. Time: O(BITS). */
    T maximize(T seed = 0) const {
        T res = seed;
        for (int i = BITS-1; i >= 0; i--) {
            if (!basis[i]) continue;
            res = max(res, T(res ^ basis[i]));
        }
        return res;
    }
};
