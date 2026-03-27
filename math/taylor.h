#pragma once
#include "core.h"

namespace taylor {

template<size_t N>
struct Series {
    array<double, N+1> a{};

    Series() { a.fill(0.0); }
    Series(double c) { a.fill(0.0); a[0] = c; }

    double& operator[](size_t i) { return a[i]; }
    double operator[](size_t i) const { return a[i]; }

    static Series variable(double value_at_0, double first_derivative = 1.0) {
        Series x(value_at_0);
        x[1] = 1.0; // x = x0 + t
        return x;
    }
};

template<size_t N>
Series<N> make_var(double value_at_0, double first_derivative = 1.0) {
    return Series<N>::variable(value_at_0, first_derivative);
}

template<size_t N>
Series<N>& operator+=(Series<N>& x, const Series<N>& y) {
    for (size_t k = 0; k <= N; k++) x[k] += y[k];
    return x;
}

template<size_t N>
Series<N>& operator-=(Series<N>& x, const Series<N>& y) {
    for (size_t k = 0; k <= N; k++) x[k] -= y[k];
    return x;
}

template<size_t N>
Series<N>& operator*=(Series<N>& x, const Series<N>& y) {
    for (size_t k = 0; k <= N; k++) x[k] *= y[k];
    return x;
}

template<size_t N>
Series<N>& operator/=(Series<N>& x, double c) {
    if (c == 0.0) throw std::runtime_error("division by zero scalar");
    for (size_t k = 0; k <= N; k++) x[k] /= c;
    return x;
}

template<size_t N>
Series<N> operator+(Series<N> x, const Series<N>& y) {
    for (size_t k = 0; k <= N; k++) x[k] += y[k];
    return x;
}

template<size_t N>
Series<N> operator-(Series<N> x, const Series<N>& y) {
    for (size_t k = 0; k <= N; k++) x[k] -= y[k];
    return x;
}

template<size_t N>
Series<N> operator-(Series<N> x) {
    for (size_t k = 0; k <= N; k++) x[k] = -x[k];
    return x;
}

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

template<size_t N>
Series<N> operator+(Series<N> x, double c) {
    x[0] += c;
    return x;
}

template<size_t N>
Series<N> operator+(double c, Series<N> x) {
    x[0] += c;
    return x;
}

template<size_t N>
Series<N> operator-(Series<N> x, double c) {
    x[0] -= c;
    return x;
}

template<size_t N>
Series<N> operator-(double c, Series<N> x) {
    x[0] -= c;
    return x;
}

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


template<size_t N>
Series<N> derivative_series(const Series<N>& x) {
    Series<N> d;
    for (size_t k = 0; k < N; k++) {
        d[k] = static_cast<double>(k+1) * x[k+1];
    }
    d[N] = 0.0;
    return d;
}

template<size_t N>
Series<N> integral_series(const Series<N>& x) {
    Series<N> i;
    i[0] = 0.0;
    for (size_t k = 0; k < N; k++) {
        i[k+1] = x[k] / static_cast<double>(k+1);
    }
    return i;
}

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

template<size_t N>
Series<N> log(const Series<N>& x) {
    if (x[0] <= 0.0) throw std::runtime_error("log requires positive constant term");
    Series<N> y = integral_series(derivative_series(x) / x);
    y[0] = std::log(x[0]);
    return y;
}


// TODO: sincos pair










} // namespace taylor
