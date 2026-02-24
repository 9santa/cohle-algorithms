#include <bits/stdc++.h>
#include <cstdlib>
using namespace std;

double conditionalProbability(double P_A_and_B, double P_B) {
    if (P_B == 0) return 0;
    return P_A_and_B / P_B;
}

// Bayes formula
double bayesTheorem(double P_B_given_A, double P_A, double P_B) {
    if (P_B == 0) return 0;
    return (P_B_given_A * P_A) / P_B;
}

double randDouble(double low, double high) {
    return low + (high - low) * ((double)rand() / RAND_MAX);
}

// Monte-Carlo's Method
double monteCarloCircle(int total_rounds) {
    srand(time(0));
    int inside = 0;
    for (int i = 0; i < total_rounds; i++) {
        double x = randDouble();
        double y = randDouble();
        if (x*x + y*y <= 1.0) inside++;
    }
    return (double)inside / total_rounds;
}


// EV (мат. ожидание, средне-взвешенное)
double expectedValue(const vector<double>& values, const vector<double>& probs = {}) {
    double res = 0.0;
    if (probs.empty()) {
        for (const double& v : values) res += v;
        return res / values.size();
    } else {
        for (size_t i = 0; i < values.size(); i++)
            res += values[i] * probs[i];
        return res;
    }
}

// Variance (дисперсия)
