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

template <class... Ks>
using RangeTreeND = typename MakeRangeTree<Ks...>::type;

} // namespace ds 