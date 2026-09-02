#pragma once
#include <bits/stdc++.h>
using namespace std;

/* Idea:
    - we have L hash-tables
    - in each hash-table - K random hyperplanes, a single hyperplane is defined by its normal vector
    - so we have K normal vectors r_i
    - for vector x calculate bit signature of length K:
        - if dot(x, r_i) >= 0 -> bit = 1
        - else bit = 0
        - so h(x) = sign(dot(x, r))
    - this K-bit signature will be the bucket id in this hashmap
    - for query:
        - calc bucket id in all hashmaps
        - gather candidates from those buckets
        - for candidates calc exact cosine similarity */

using u32 = uint32_t;
using u64 = uint64_t;

/** Locality-sensitive hashing for approximate cosine nearest neighbors. Space: O(LKdim + nL). */
class CosineLSH {
public:
    using Vec = vector<float>;

    /** Creates L hash tables, each with K random hyperplanes. Time: O(LKdim). */
    CosineLSH(int dim_, int num_tables, int num_hyperplanes, u32 seed = 42)
        : dim(dim_), L(num_tables), K(num_hyperplanes), rng(seed) {
        if (K <= 0 || K > 63) {
            throw invalid_argument("K must be in [1, 64] for uint64_t signature.");
        }
        generate_hyperplanes();
    }

    // Build from dataset
    /** Builds hash tables from data vectors. Time: O(nL K dim). */
    void build(const vector<Vec>& data) {
        dat.clear();
        dat.reserve(data.size());

        for (const auto& v : data) {
            if ((int)v.size() != dim) {
                throw invalid_argument("All vectors must have the same dimension = dim.");
            }
            dat.push_back(normalize(v));
        }

        tables.assign(L, {});
        for (int id = 0; id < (int)dat.size(); id++) {
            for (int t = 0; t < L; t++) {
                u64 key = hash_vector(t, dat[id]);
                tables[t][key].push_back(id);
            }
        }
    }

    // Returns top_k nearest neighbors by cosine similarity
    /** Returns approximate top_k neighbors by cosine similarity. Time: O(LKdim + candidates * dim). */
    vector<pair<int, float>> k_nearest(const Vec& q, int top_k = 5) const {
        if ((int)q.size() != dim) {
            throw invalid_argument("Query vector has wrong dimension != dim.");
        }
        if (dat.empty()) return {};

        Vec nq = normalize(q);

        vector<int> seen(dat.size(), 0);
        vector<int> candidates;
        candidates.reserve(250);

        for (int t = 0; t < L; t++) {
            u64 key = hash_vector(t, nq);
            auto it = tables[t].find(key);
            if (it == tables[t].end()) continue;

            for (int id : it->second) {
                if (!seen[id]) {
                    seen[id] = 1;
                    candidates.push_back(id);
                }
            }
        }

        vector<pair<int, float>> scored;
        scored.reserve(candidates.size());

        for (int id : candidates) {
            float sim = dot(nq, dat[id]); // since both are normalized
            scored.push_back({id, sim});
        }

        // put top_k largest similarities to the beginning
        if ((int)scored.size() > top_k) {
            nth_element(scored.begin(), scored.begin() + top_k, scored.end(),
                        [](const auto& a, const auto& b) {
                                    return a.second > b.second;
                              });
            scored.resize(top_k);
        }

        // sort them descending
        sort(scored.begin(), scored.end(),
             [](const auto& a, const auto& b){
                         return a.second > b.second;
                    });

        return scored;
    }

private:
    int dim;
    int L;
    int K;
    mutable mt19937 rng;

    vector<Vec> dat;

    // hyperplanes[table][k][dim]
    vector<vector<Vec>> hyperplanes;

    // tables[table][signature] = list of point ids in buckets
    vector<unordered_map<u64, vector<int>>> tables;

    /** Returns dot product. Time: O(dim). */
    static float dot(const Vec& a, const Vec& b) {
        float s = 0.0f;
        for (int i = 0; i < (int)a.size(); i++) s += a[i] * b[i];
        return s;
    }

    /** Returns Euclidean norm. Time: O(dim). */
    static float norm(const Vec& v) {
        return sqrt(dot(v, v));
    }

    /** Returns normalized vector, or original zero vector. Time: O(dim). */
    static Vec normalize(const Vec& v) {
        float n = norm(v);
        if (n == 0.0f) return v; // null vector
        Vec out = v;
        for (auto& x : out) x/= n;
        return out;
    }

    /** Generates random hyperplane normals. Time: O(LKdim). */
    void generate_hyperplanes() {
        normal_distribution<float> dist(0.0f, 1.0f);

        hyperplanes.assign(L, vector<Vec>(K, Vec(dim)));
        for (int t = 0; t < L; t++) {
            for (int k = 0; k < K; k++) {
                for (int d = 0; d < dim; d++) {
                    hyperplanes[t][k][d] = dist(rng);
                }
            }
        }
    }

    /** Returns K-bit signature of v for one table. Time: O(Kdim). */
    u64 hash_vector(int table_id, const Vec& v) const {
        u64 sig = 0;
        for (int k = 0; k < K; k++) {
            float proj = dot(v, hyperplanes[table_id][k]);
            if (proj >= 0.0f) {
                sig |= (1ULL << k);
            }
        }
        return sig;
    }
};
