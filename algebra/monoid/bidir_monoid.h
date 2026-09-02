#pragma once
#include <bits/stdc++.h>
using namespace std;

/** Detects whether a value type exposes a fail field. */
template<class T, class = void>
struct has_fail : std::false_type {};
template<class T>
struct has_fail<T, std::void_t<decltype(std::declval<T>().fail)>> : std::true_type {};

/** Stores forward and reverse aggregate values, with optional fail propagation. */
template<class X, bool HasFail>
struct BidirValImpl;

template<class X>
struct BidirValImpl<X, false> { X fwd, rev; };

template<class X>
struct BidirValImpl<X, true> { X fwd, rev; bool fail = false; };

template<class X>
using BidirVal = BidirValImpl<X, has_fail<X>::value>;

/** Bidirectional wrapper for any associative monoid. Space: O(1). */
template<class Monoid>
struct Monoid_Bidir {
    using X0 = typename Monoid::value_type;
    using value_type = BidirVal<X0>;
    using X = value_type;

    /** Returns the bidirectional identity. Time: O(Monoid::id). */
    static X id() {
        X res;
        res.fwd = Monoid::id();
        res.rev = Monoid::id();
        if constexpr (has_fail<X0>::value) {
            res.fail = false;
        }
        return res;
    }

    /** Combines adjacent bidirectional aggregates. Time: O(Monoid::op). */
    static X op(const X& a, const X& b) {
        X res;
        res.fwd = Monoid::op(a.fwd, b.fwd);
        res.rev = Monoid::op(b.rev, a.rev);
        if constexpr (has_fail<X0>::value) {
            res.fail = a.fail || b.fail || res.fwd.fail || res.rev.fail;
        }
        return res;
    }
};
