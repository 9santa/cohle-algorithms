#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Removes index idx from the current Mo window. */
void remove(int idx);
/** Adds index idx to the current Mo window. */
void add(int idx);
/** Returns the current answer. */
int get_ans();

int BLOCK_SIZE;

struct Query {
    int l, r, idx;

    // zig-zag trick
    bool operator<(const Query& rhs) const {
        int block_a = this->l / BLOCK_SIZE;
        int block_b = rhs.l / BLOCK_SIZE;

        if (block_a != block_b)
            return block_a < block_b;   // different L-block: only blocks matter

        if (block_a & 1)
            return this->r > rhs.r;
        else
            return this->r < rhs.r;
    }
};

/** Answers offline static range queries using Mo's algorithm. Time: O((N + Q) sqrt N) for O(1) add/remove. */
vector<int> mo_algorithm(vector<Query> qs) {
    vector<int> ans(qs.size());
    sort(qs.begin(), qs.end());

    // TODO: initialize data structure

    int curl = 0;
    int curr = -1;
    // invariant: data structure will always reflect the range [curl, curr]
    for (auto q : qs) {
        while (curl > q.l) {
            curl--;
            add(curl);
        }
        while (curr < q.r) {
            curr++;
            add(curr);
        }
        while (curl < q.l) {
            remove(curl);
            curl++;
        }
        while (curr > q.r) {
            remove(curr);
            curr--;
        }
        ans[q.idx] = get_ans();
    }
    return ans;
}
