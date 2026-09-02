#pragma once
#include <bits/stdc++.h>
using namespace std;

namespace taylor {

/** Truncated Taylor series with coefficients through degree N. Space: O(N). */
template<size_t N>
struct Series {
    array<double, N+1> a{};

    Series() { a.fill(0.0); }
    Series(double c) { a.fill(0.0); a[0] = c; }

    double& operator[](size_t i) { return a[i]; }
    double operator[](size_t i) const { return a[i]; }

    /** Creates variable series x0 + t. Time: O(N). */
    static Series variable(double value_at_0, double first_derivative = 1.0) {
        Series x(value_at_0);
        x[1] = 1.0; // x = x0 + t
        return x;
    }
};

/** Creates variable series x0 + t. Time: O(N). */
template<size_t N>
Series<N> make_var(double value_at_0, double first_derivative = 1.0) {
    return Series<N>::variable(value_at_0, first_derivative);
}

/** Adds y into x coefficient-wise. Time: O(N). */
template<size_t N>
Series<N>& operator+=(Series<N>& x, const Series<N>& y) {
    for (size_t k = 0; k <= N; k++) x[k] += y[k];
    return x;
}

/** Subtracts y from x coefficient-wise. Time: O(N). */
template<size_t N>
Series<N>& operator-=(Series<N>& x, const Series<N>& y) {
    for (size_t k = 0; k <= N; k++) x[k] -= y[k];
    return x;
}

/** Multiplies coefficients pointwise. Time: O(N). */
template<size_t N>
Series<N>& operator*=(Series<N>& x, const Series<N>& y) {
    for (size_t k = 0; k <= N; k++) x[k] *= y[k];
    return x;
}

/** Divides all coefficients by scalar c. Time: O(N). */
template<size_t N>
Series<N>& operator/=(Series<N>& x, double c) {
    if (c == 0.0) throw std::runtime_error("division by zero scalar");
    for (size_t k = 0; k <= N; k++) x[k] /= c;
    return x;
}

/** Returns x + y coefficient-wise. Time: O(N). */
template<size_t N>
Series<N> operator+(Series<N> x, const Series<N>& y) {
    for (size_t k = 0; k <= N; k++) x[k] += y[k];
    return x;
}

/** Returns x - y coefficient-wise. Time: O(N). */
template<size_t N>
Series<N> operator-(Series<N> x, const Series<N>& y) {
    for (size_t k = 0; k <= N; k++) x[k] -= y[k];
    return x;
}

/** Returns -x. Time: O(N). */
template<size_t N>
Series<N> operator-(Series<N> x) {
    for (size_t k = 0; k <= N; k++) x[k] = -x[k];
    return x;
}

/** Returns truncated product x*y. Time: O(N^2). */
template<size_t N>
Series<N> operator*(const Series<N>& x, const Series<N>& y) {
    Series<N> z;
    for (size_t k = 0; k <= N; k++) {
        double s = 0.0;
        for (size_t i = 0; i <= k; i++) s += x[i] * y[k-i];
        z[k] = s;
    }
    return z;
}

/** Adds a constant to a series. Time: O(1). */
template<size_t N>
Series<N> operator+(Series<N> x, double c) {
    x[0] += c;
    return x;
}

/** Adds a constant to a series. Time: O(1). */
template<size_t N>
Series<N> operator+(double c, Series<N> x) {
    x[0] += c;
    return x;
}

/** Subtracts a constant from a series. Time: O(1). */
template<size_t N>
Series<N> operator-(Series<N> x, double c) {
    x[0] -= c;
    return x;
}

/** Returns constant c minus series x. Time: O(1). */
template<size_t N>
Series<N> operator-(double c, Series<N> x) {
    x[0] -= c;
    return x;
}

/** Returns truncated quotient x/y. Time: O(N^2). */
template<size_t N>
Series<N> operator/(const Series<N>& x, const Series<N>& y) {
    if (y[0] == 0.0) throw std::runtime_error("division by series with zero constant term");

    Series<N> q;
    q[0] = x[0] / y[0];
    for (size_t k = 1; k <= N; k++) {
        double s = 0.0;
        for (size_t i = 1; i <= k; i++) {
            s += y[i] * q[k-i];
        }
        q[k] = (x[k] - s) / y[0];
    }
    return q;
}


/** Returns the derivative series. Time: O(N). */
template<size_t N>
Series<N> derivative_series(const Series<N>& x) {
    Series<N> d;
    for (size_t k = 0; k < N; k++) {
        d[k] = static_cast<double>(k+1) * x[k+1];
    }
    d[N] = 0.0;
    return d;
}

/** Returns the integral series with zero constant. Time: O(N). */
template<size_t N>
Series<N> integral_series(const Series<N>& x) {
    Series<N> i;
    i[0] = 0.0;
    for (size_t k = 0; k < N; k++) {
        i[k+1] = x[k] / static_cast<double>(k+1);
    }
    return i;
}

/** Returns exp(x) as a truncated series. Time: O(N^2). */
template<size_t N>
Series<N> exp(const Series<N>& x) {
    Series<N> y;
    y[0] = std::exp(x[0]);

    for (size_t k = 1; k <= N; k++) {
        double s = 0.0;
        for (size_t i = 1; i <= k; i++) {
            s += static_cast<double>(i) * x[i] * y[k-i];
        }
        y[k] = s / static_cast<double>(k);
    }
    return y;
}

/** Returns log(x) as a truncated series. Time: O(N^2). */
template<size_t N>
Series<N> log(const Series<N>& x) {
    if (x[0] <= 0.0) throw std::runtime_error("log requires positive constant term");
    Series<N> y = integral_series(derivative_series(x) / x);
    y[0] = std::log(x[0]);
    return y;
}


// TODO: sincos pair


} // namespace taylor
