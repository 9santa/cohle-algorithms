#pragma once
#include "../header.h"
#include <cmath>
#include <vector>

namespace linalg {

/*
    Lightweight vector math:

    - N-dimensional vectors (template parameter N)
    - dot product, norms
    - projection onto another vector
    - Gram–Schmidt orthonormalization
    - 3D-only: cross product, scalar triple product
    - geometry helpers: triangle area, tetrahedron volume

    Default scalar type is double.
*/

template<int N, class T = double>
struct Vec {
    std::array<T, N> a{};

    T& operator[](int i) { return a[(size_t)i]; }
    const T& operator[](int i) const { return a[(size_t)i]; }

    Vec& operator+=(const Vec& o) { for (int i=0;i<N;i++) a[i] += o[i]; return *this; }
    Vec& operator-=(const Vec& o) { for (int i=0;i<N;i++) a[i] -= o[i]; return *this; }
    Vec& operator*=(T k) { for (auto& x : a) x *= k; return *this; }
    Vec& operator/=(T k) { for (auto& x : a) x /= k; return *this; }
};

// -------------------------------- dot / norms --------------------------------

/*
    Dot product:
      dot(x,y) = Σ x_i * y_i
*/
template<int N, class T>
inline T dot(const Vec<N,T>& x, const Vec<N,T>& y) {
    T res{};
    for (int i = 0; i < N; i++) res += x[i] * y[i];
    return res;
}

/*
    Squared Euclidean norm:
      ||x||^2 = dot(x,x)
*/
template<int N, class T>
inline T norm2(const Vec<N,T>& x) { return dot(x, x); }

/*
    Euclidean norm:
      ||x|| = sqrt(dot(x,x))
*/
template<int N, class T>
inline T norm(const Vec<N,T>& x) {
    return (T)std::sqrt((long double)norm2(x));
}


// -------------------------------- projection --------------------------------

/*
    Projection of x onto vector 'onto':

      proj(x onto onto) = onto * (dot(x,onto) / dot(onto,onto))

    If 'onto' is (near) zero, returns the zero vector.
*/
template<int N, class T>
inline Vec<N,T> proj(const Vec<N,T>& x, const Vec<N,T>& onto) {
    T denom = dot(onto, onto);
    if (fabsl((long double)denom) <= 1e-18L) return Vec<N,T>{};
    return dot(x, onto) / denom * onto;
}

// -------------------------------- 3D operations --------------------------------

/*
    Cross product (3D only):
      cross(a,b) = a × b = S of some area = det(M), where
      M = |i   j  k|
          |Xa Xb Xz|
          |Ya Yb Yz|
*/
template<class T>
inline Vec<3, T> cross(const Vec<3, T>& a, const Vec<3, T>& b) {
    return Vec<3, T>{{ a[1]*b[2] - a[2]*b[1],
                       a[2]*b[0] - a[0]*b[2],
                       a[0]*b[1] - a[1]*b[0] }};
}

/*
    Scalar triple product:
      triple(a,b,c) = a · (b × c)

    Geometric meaning:
      |triple| = volume of the parallelepiped spanned by (a,b,c)
*/
template<class T>
inline T triple(const Vec<3,T>& a, const Vec<3,T>& b, const Vec<3,T>& c) {
    return dot(a, cross(b, c));
}

/*
    Triangle area in 3D:
      area(ABC) = 0.5 * || (B-A) × (C-A) ||
*/
template<class T>
inline T triangle_area(const Vec<3,T>& A, const Vec<3,T>& B, const Vec<3,T>& C){
    return (T)0.5 * norm(cross(B - A, C - A));
}

/*
    Tetrahedron volume in 3D:
      vol(ABCD) = | (B-A) · ((C-A) × (D-A)) | / 6
*/
template<class T>
inline T tetra_volume(const Vec<3,T>& A, const Vec<3,T>& B,
                      const Vec<3,T>& C, const Vec<3,T>& D){
    return (T)(fabsl((long double)triple(B - A, C - A, D - A)) / 6.0L);
}

// -------------------------------- Gram–Schmidt --------------------------------

/*
    Gram–Schmidt orthonormalization.

    Input: a list of vectors v1..vk
    Output: an orthonormal basis u1..ur (r <= k)

    Steps:
      u1 = normalize(v1)
      u2 = normalize(v2 - proj(v2,u1))
      ...
    'eps' is used to drop nearly-zero vectors (linear dependence / numeric issues).
*/
template<int N, class T = double>
    inline vector<Vec<N,T>> gram_schmidt(const vector<Vec<N,T>>& v, T eps = (T)1e-12) {
    vector<Vec<N,T>> basis;
    for (auto x : v) {
        for (auto& u : basis) x -= proj(x, u);
        T nrm = norm(x);
        if ((long double)nrm > (long double)eps) {
            x /= nrm;
            basis.push_back(x);
        }
    }
    return basis;
}

} // namespace linalg
