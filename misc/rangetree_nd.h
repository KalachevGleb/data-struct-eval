#pragma once

#include <set>
#include <functional>
#include <tuple>
#include <iterator>
#include "rangetree.h"

namespace ds {

// Leaf structure: 1D set with range [l;r) iteration
template <class K, class Cmp = std::less<K>>
class SetD {
public:
    void insert(const K& k){ s_.insert(k); }
    void remove(const K& k){ auto it = s_.find(k); if (it != s_.end()) s_.erase(it); }
    bool empty() const { return s_.empty(); }

    struct Range {
        using It = typename std::set<K,Cmp>::const_iterator;
        It a, b;
        It begin() const { return a; }
        It end() const { return b; }
    };
    Range range(const K& l, const K& r) const{
        return Range{ s_.lower_bound(l), s_.lower_bound(r) };
    }
private:
    std::set<K,Cmp> s_;
};

// Helper for specifying a dimension type and its comparator
template <class K, class Cmp = std::less<K>>
struct Dim { using Key = K; using CmpT = Cmp; };

// Primary template to build D-dimensional range tree from Dims<...>
template <class... Dims>
struct MakeRangeTreeDims;

// Base case: last dimension is a SetD
template <class KD, class CmpD>
struct MakeRangeTreeDims<Dim<KD, CmpD>>{
    using type = SetD<KD, CmpD>;
};

// Recursive case: outer RangeTree over Ki with Bi=Ki, comparator Ci, and Sub from rest
template <class K1, class C1, class... RestDims>
struct MakeRangeTreeDims<Dim<K1, C1>, RestDims...> {
    using SubT = typename MakeRangeTreeDims<RestDims...>::type;
    using type = RangeTree<K1, K1, C1, SubT>;
};

// Convenience builder from key types with default std::less for each
template <class... Keys>
struct MakeRangeTree;

// Base
template <class KD>
struct MakeRangeTree<KD> {
    using type = SetD<KD, std::less<KD>>;
};

// Recursive
template <class K1, class... Rest>
struct MakeRangeTree<K1, Rest...> {
    using SubT = typename MakeRangeTree<Rest...>::type;
    using type = RangeTree<K1, K1, std::less<K1>, SubT>;
};

// Alias for convenience using defaults
template <class... Ks>
using RangeTreeND = typename MakeRangeTree<Ks...>::type;

//================= Points (tuples) at leaf ==================

// Comparator by last element of tuple, tie-broken by the whole tuple lexicographically
template <class Tuple>
struct CmpByLastTuple{
    static constexpr std::size_t D = std::tuple_size<Tuple>::value;
    bool operator()(const Tuple& a, const Tuple& b) const{
        const auto& la = std::get<D-1>(a);
        const auto& lb = std::get<D-1>(b);
        if (la < lb) return true;
        if (lb < la) return false;
        return a < b;
    }
};

// Build a boundary tuple with last set to 'last' and others default-constructed
template <class Tuple, std::size_t... I>
Tuple make_boundary_with_last_impl(const typename std::tuple_element<std::tuple_size<Tuple>::value-1, Tuple>::type& last, std::index_sequence<I...>){
    return Tuple( (I == std::tuple_size<Tuple>::value-1 ? last : typename std::tuple_element<I, Tuple>::type())... );
}

template <class Tuple>
Tuple make_boundary_with_last(const typename std::tuple_element<std::tuple_size<Tuple>::value-1, Tuple>::type& last){
    return make_boundary_with_last_impl<Tuple>(last, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
}

// Leaf that stores full points (tuples), ranges by last coordinate
template <class Tuple>
class SetTupleLeaf{
public:
    void insert(const Tuple& pt){ s_.insert(pt); }
    void remove(const Tuple& pt){ auto it = s_.find(pt); if (it != s_.end()) s_.erase(it); }
    bool empty() const { return s_.empty(); }

    struct Range{
        using It = typename std::set<Tuple, CmpByLastTuple<Tuple>>::const_iterator;
        It a, b;
        It begin() const { return a; }
        It end() const { return b; }
    };
    using LastT = typename std::tuple_element<std::tuple_size<Tuple>::value-1, Tuple>::type;
    Range range(const LastT& l, const LastT& r) const{
        Tuple bl = make_boundary_with_last<Tuple>(l);
        Tuple br = make_boundary_with_last<Tuple>(r);
        return Range{ s_.lower_bound(bl), s_.lower_bound(br) };
    }
private:
    std::set<Tuple, CmpByLastTuple<Tuple>> s_;
};

// Unified sub-chain that always accepts (tuple, next-dim keys...)
template <class Tuple, class... Keys>
class SubChain;

// Base: last dimension KD — keep leaf of tuples
template <class Tuple, class KD>
class SubChain<Tuple, KD>{
public:
    void insert(const Tuple& pt, const KD&){ leaf_.insert(pt); }
    void remove(const Tuple& pt, const KD&){ leaf_.remove(pt); }
    auto range(const KD& l, const KD& r) const { return leaf_.range(l, r); }
    bool empty() const { return leaf_.empty(); }
private:
    SetTupleLeaf<Tuple> leaf_;
};

// Recursive: dimension K followed by more
template <class Tuple, class K, class Knext, class... Rest>
class SubChain<Tuple, K, Knext, Rest...>{
public:
    void insert(const Tuple& pt, const K& k, const Knext& kn, const Rest&... rs){ rt_.insert(k, pt, kn, rs...); }
    void remove(const Tuple& pt, const K& k, const Knext& kn, const Rest&... rs){ rt_.remove(k, pt, kn, rs...); }
    template <class... Xs>
    auto range(const K& l, const K& r, const Xs&... xs) const { return rt_.range(l, r, xs...); }
    bool empty() const { return false; }
private:
    RangeTree<K, K, std::less<K>, SubChain<Tuple, Knext, Rest...>> rt_;
};

// High-level wrapper that keeps full point tuples but uses RangeTree internally
template <class K1, class K2, class... Rest>
class RangeTreeNDPoints{
public:
    using Tuple = std::tuple<K1, K2, Rest...>;
    using HeadSub = SubChain<Tuple, K2, Rest...>;
    using Head = RangeTree<K1, K1, std::less<K1>, HeadSub>;

    // insert/remove without tuple duplication
    void insert(const K1& k1, const K2& k2, const Rest&... ks){ Tuple pt(k1,k2,ks...); head_.insert(k1, pt, k2, ks...); }
    void remove(const K1& k1, const K2& k2, const Rest&... ks){ Tuple pt(k1,k2,ks...); head_.remove(k1, pt, k2, ks...); }

    // range returns lazy iterable of tuples
    template <class... Bounds>
    auto range(const Bounds&... b) const { return head_.range(b...); }

    // access underlying head if needed
    const Head& head() const { return head_; }
    Head& head() { return head_; }
private:
    Head head_;
};

// Alias helper
template <class... Keys>
using RangeTreeNDPointsT = RangeTreeNDPoints<Keys...>;

} // namespace ds 