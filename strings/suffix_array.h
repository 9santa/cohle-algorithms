#pragma once
#include "../header.h"

struct SuffixArray {
    int n;
    const int SIGMA;
    vector<int> p;
    vector<int> c;
    int classes = 1;

    SuffixArray(const string& s, int _SIGMA=256) : n(sz(s)), SIGMA(_SIGMA) {
        build(s);
    }

    // O(n log n)
    void build(string s) {
        s += '$';
        classes = 1;
        vector<int> cnt(max(SIGMA, n), 0);
        for (int i = 0; i < n; i++) cnt[s[i]]++;
        for (int i = 1; i < SIGMA; i++) cnt[i] += cnt[i-1];
        p.resize(n, 0);
        for (int i = n-1; i >= 0; i--) {
            p[--cnt[s[i]]] = i;
        }
        c.resize(n, 0);
        c[p[0]] = 0;
        for (int i = 1; i < n; i++) {
            if (s[p[i]] != s[p[i-1]]) {
                classes++;
            }
            c[p[i]] = classes - 1;
        }

        vector<int> pn(n), cn(n);
        for (int h = 0; (1 << h) < n; h++) {
            for (int i = 0; i < n; i++) {
                pn[i] = p[i] - (1 << h);
                if (pn[i] < 0) pn[i] += n;
            }
            fill(cnt.begin(), cnt.begin()+classes, 0);
            for (int i = 0; i < n; i++) {
                cnt[c[pn[i]]]++;
            }
            for (int i = 1; i < n; i++) {
                cnt[i] += cnt[i-1];
            }
            for (int i = n-1; i >= 0; i--) {
                p[--cnt[c[pn[i]]]] = pn[i];
            }

            cn[p[0]] = 0;
            classes = 1;
            for (int i = 1; i < n; i++) {
                pair<int, int> cur = {c[p[i]], c[(p[i] + (1 << h)) % n]};
                pair<int, int> prev = {c[p[i-1]], c[(p[i-1] + (1 << h)) % n]};
                if (cur != prev) {
                    classes++;
                }
                cn[p[i]] = classes - 1;
            }
            c.swap(cn);
        }

        p.erase(p.begin()); // remove p[0] = n = |s|
    }



};

