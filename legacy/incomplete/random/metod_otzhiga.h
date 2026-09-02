#pragma once
#include <bits/stdc++.h>
using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
double random_uniform_double() {
    return uniform_real_distribution<double>(0.0, 1.0)(rng);
}

// This is for minimization, for maximization - symmetrical
void method() {
    struct State {}; // State of the problem

    const int MAX_ITER = 1e7;
    const double COOLING_RATE = 0.99995;
    const double TIME_LIMIT = 1.95;
    double temp = 10000.0;
    auto start = clock();

    State cur = initialState();
    State best = cur;
    double cur_score = score(cur);
    double best_score = cur_score;

    for (int iter = 0; iter < MAX_ITER; iter++) {
        if (iter % 100 == 0) {
            auto now = clock();
            double elapsed = double(now - start) / CLOCKS_PER_SEC;
            if (elapsed > TIME_LIMIT) {
                break;
            }
        }

        State next = randModifyState(cur);

        double next_score = score(next);
        double delta = next_score - cur_score;
        if (delta < 0 || exp(-delta / temp) > random_uniform_double()) {
            cur = next;
            cur_score = next_score;

            if (cur_score < best_score) {
                best = cur;
                best_score = cur_score;
            }
        }

        temp *= COOLING_RATE;
    }
}

/* N-Queens Problem
State:
    permutation p, where p[row] = col
Score:
    number of attacking queen pairs
Goal:
    minimize score to 0
Mutation:
    swap two columns
Stop:
    score == 0 or time limit
*/
namespace nqueens {
int n;
using State = vector<int>;

State initialState() {
    State p(n);
    iota(p.begin(), p.end(), 0);
    shuffle(p.begin(), p.end(), rng);
    return p;
}

int score(const State& p) {
    int conflicts = 0;
    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            if (abs(i-j) == abs(p[i] - p[j])) {
                conflicts++;
            }
        }
    }
    return conflicts;
}

// O(n) diagonal conflicts search
int score1(const State& p) {
    vector<int> diag1(2*n), diag2(2*n);
    for (int row = 0; row < n; row++) {
        int col = p[row];
        diag1[row + col]++;
        diag2[row - col + n]++;
    }

}

// Here we return the modified copy, so that the rollback is easy
State randModifyState(const State& p) {
    auto modified = p;
    int i = rng() % modified.size();
    int j = rng() % modified.size();
    while (i == j) j = rng() % modified.size();

    swap(modified[i], modified[j]);
    return modified;
}

} // namespace nqueens


/* Knapsack / generic subset optimization
State:
    bitmask
Score:
    objective - penalties
Mutation:
    flip one bit, flip k bits, swap 1/0
Goal:
    maximize or minimize depending on task

Задачи типа выбрать подмножество:
    выбрать подмножество вершин;
    выбрать набор предметов;
    выбрать набор отрезков;
    выбрать набор задач;
    выбрать набор признаков;
    выбрать набор рёбер;
    выбрать subset с ограничениями.
*/
namespace subsets {
using State = vector<int>;
const long long PENALTY_COEF = 1000000LL;
int n;
vector<int> weight;
vector<int> value;
long long capacity;

long long score(const State& take) {
    long long total_weight = 0;
    long long total_value = 0;

    for (int i = 0; i < n; i++) {
        if (take[i]) {
            total_weight += weight[i];
            total_value += value[i];
        }
    }

    long long penalty = 0;
    if (total_weight > capacity) {
        penalty = PENALTY_COEF * (total_weight - capacity);
    }

    return total_value - penalty;
}

State randModifyState(const State& take) {
    auto modified = take;
    int i = rng() % n;
    modified[i] ^= 1;
    return modified;
}


} // namespace subsets
