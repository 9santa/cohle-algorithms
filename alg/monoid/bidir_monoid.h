#pragma once
#include <type_traits>

template<class T, class = void>
struct has_fail : std::false_type {};
template<class T>
struct has_fail<T, std::void_t<decltype(std::declval<T>().fail)>> : std::true_type {};

template<class X, bool HasFail>
struct BidirValImpl;

template<class X>
struct BidirValImpl<X, false> { X fwd, rev; };

template<class X>
struct BidirValImpl<X, true> { X fwd, rev; bool fail = false; };

template<class X>
using BidirVal = BidirValImpl<X, has_fail<X>::value>;

template<class Monoid>
struct Monoid_Bidir {
    using X0 = typename Monoid::value_type;
    using value_type = BidirVal<X0>;
    using X = value_type;

    static X id() {
        X res;
        res.fwd = Monoid::id();
        res.rev = Monoid::id();
        if constexpr (has_fail<X0>::value) {
            res.fail = false;
        }
        return res;
    }

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
