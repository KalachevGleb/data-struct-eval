#include <catch2/catch_test_macros.hpp>
#include "rbtree.h"
#include "avltree.h"
#include "23Tree.h"
#include "Btree.h"
#include "BtreeT.h"
#include "priority_queue.h"
#include "stringmap.h"
#include "pst.h"
#include "sort.h"
#include <vector>
#include <algorithm>
#include "rangetree.h"
#include <set>
#include <climits>
#include <random>
#include "rangetree_nd.h"

namespace {
struct Point { int x; int y; };

struct CmpX {
    bool operator()(const Point& a, const Point& b) const {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    }
};
struct CmpY {
    bool operator()(const Point& a, const Point& b) const { return a.y < b.y; }
    bool operator()(const Point& a, const int& by) const { return a.y < by; }
};

struct SetByX {
    std::set<Point, CmpX> s;
    void insert(const Point& p){ s.insert(p); }
    void remove(const Point& p){ auto it = s.find(p); if (it != s.end()) s.erase(it); }
    bool empty() const { return s.empty(); }
    size_t count_in(int lx, int rx) const{
        Point l{lx, INT_MIN}, r{rx, INT_MIN};
        return std::distance(s.lower_bound(l), s.lower_bound(r));
    }
    struct Range {
        using It = std::set<Point, CmpX>::const_iterator;
        It a, b;
        It begin() const { return a; }
        It end() const { return b; }
    };
    Range range(int lx, int rx) const{
        Point l{lx, INT_MIN}, r{rx, INT_MIN};
        return Range{ s.lower_bound(l), s.lower_bound(r) };
    }
};
} // namespace

TEST_CASE("Red-Black tree basic operations") {
    RBT<int,int> tree;
    tree.insert(2,20);
    tree.insert(1,10);
    tree.insert(3,30);
    REQUIRE(tree.find(2).isValid());
    REQUIRE(tree.find(2).value() == 20);
    tree.remove(2);
    REQUIRE_FALSE(tree.find(2).isValid());
}

TEST_CASE("AVL tree basic operations") {
    AVLTree<int,int> tree;
    tree.insert(5,50);
    tree.insert(2,20);
    tree.insert(8,80);
    REQUIRE(tree.find(2).isValid());
    REQUIRE(tree.find(2).value() == 20);
    tree.remove(2);
    REQUIRE_FALSE(tree.find(2).isValid());
}

TEST_CASE("2-3 tree basic operations") {
    Tree23<int,int,RBTBase::_default_cmp<int>> tree;
    tree.insert(1,10);
    tree.insert(4,40);
    tree.insert(2,20);
    auto p = tree.find(4);
    REQUIRE(p != nullptr);
    REQUIRE(*p == 40);
    tree.remove(4);
    REQUIRE(tree.find(4) == nullptr);
}

TEST_CASE("BTreeV basic operations") {
    BTreeV<int,int> tree;
    tree.setB(4);
    tree.insert(3,30);
    tree.insert(1,10);
    tree.insert(2,20);
    auto it = tree.find(2);
    REQUIRE(it.isValid());
    tree.remove(2);
    REQUIRE_FALSE(tree.find(2).isValid());
}

TEST_CASE("BTreeT basic operations") {
    BTreeT<int,int,4,RBTBase::_default_cmp<int>> tree;
    tree.insert(3,30);
    tree.insert(1,10);
    tree.insert(2,20);
    auto it = tree.find(1);
    REQUIRE(it.isValid());
    tree.remove(1);
    REQUIRE_FALSE(tree.find(1).isValid());
}

TEST_CASE("StringTree basic operations") {
    StringTree<int> tr;
    tr.insert("a",1);
    tr.insert("b",2);
    auto it2 = tr.find("a");
    REQUIRE(it2.isValid());
    REQUIRE(*it2 == 1);
    tr.remove("a");
    REQUIRE_FALSE(tr.find("a").isValid());
}

TEST_CASE("PriorityQueue basic operations") {
    PriorityQueue pq;
    pq.init(3,10);
    pq[0]+=2;
    pq[1]+=3;
    REQUIRE(pq.maxW()==3);
    pq[1]-=3;
    REQUIRE(pq.maxW()==2);
}

TEST_CASE("PST basic operations") {
    PST<int> pst;
    pst.insert(1,1,100);
    pst.insert(2,2,200);
    auto p = pst.minY(1,2,2);
    REQUIRE(p.isValid());
    REQUIRE(p.value()==100);
    pst.remove(1,1);
    REQUIRE_FALSE(pst.minY(1,1,1).isValid());
}

struct int_less {
    bool operator()(int a, int b) const { return a<b; }
};

TEST_CASE("Sorting algorithms") {
    std::vector<int> vals = {3,1,4,2};
    auto v1 = vals;
    quick_sort(v1.data(), v1.size(), int_less());
    REQUIRE(std::is_sorted(v1.begin(), v1.end()));
    auto v2 = vals;
    heap_sort(v2.data(), v2.size(), int_less());
    REQUIRE(std::is_sorted(v2.begin(), v2.end()));
    auto v3 = vals; std::vector<int> tmp(v3.size());
    merge_sort(v3.data(), tmp.data(), v3.size(), int_less());
    REQUIRE(std::is_sorted(v3.begin(), v3.end()));
}

TEST_CASE("RangeTree 2D with outer-by-Y and inner-by-X") {
    using RT = ds::RangeTree<Point,int,CmpY,SetByX>;
    RT rt{CmpY{}};

    // insert points (x,y)
    rt.insert(Point{10,1}, Point{10,1});
    rt.insert(Point{15,1}, Point{15,1});
    rt.insert(Point{20,2}, Point{20,2});
    rt.insert(Point{30,3}, Point{30,3});

    // find_key by y: any point with same y is equal under outer comparator
    auto *s1 = rt.find_key(Point{0,1});
    REQUIRE(s1 != nullptr);
    REQUIRE(s1->count_in(0, 100) == 2);

    // visit_cover over y in [1;3) and x in [0;100)
    size_t total = 0;
    rt.visit_cover(1,3,[&](const SetByX& sub){ total += sub.count_in(0,100); });
    REQUIRE(total == 3); // y=1 and y=2

    // remove one point
    rt.remove(Point{10,1}, Point{10,1});
    s1 = rt.find_key(Point{0,1});
    REQUIRE(s1 != nullptr);
    REQUIRE(s1->count_in(0, 100) == 1);

    // iterate rectangle [y in 1;4) x [x in 0;100)
    std::vector<Point> v;
    for (auto p : rt.range(1,4, 0,100)) v.push_back(p);
    std::sort(v.begin(), v.end(), [](const Point& a, const Point& b){ return a.x < b.x || (a.x==b.x && a.y<b.y); });
    REQUIRE(v.size() == 3);
    REQUIRE(v[0].x == 15);
    REQUIRE(v[0].y == 1);
    REQUIRE(v[1].x == 20);
    REQUIRE(v[1].y == 2);
    REQUIRE(v[2].x == 30);
    REQUIRE(v[2].y == 3);
}

TEST_CASE("RangeTree randomized against naive for 2D rectangles") {
    using RT = ds::RangeTree<Point,int,CmpY,SetByX>;
    RT rt{CmpY{}};

    struct FullCmp {
        bool operator()(const Point& a, const Point& b) const{
            if (a.y != b.y) return a.y < b.y;
            if (a.x != b.x) return a.x < b.x;
            return false;
        }
    };
    std::set<Point, FullCmp> base;

    std::mt19937 rng(1234567);
    auto rnd = [&](int lo, int hi){ std::uniform_int_distribution<int> d(lo, hi); return d(rng); };

    auto check_once = [&](){
        int y1 = rnd(0, 1000);
        int y2 = rnd(y1, 1001);
        int x1 = rnd(0, 1000);
        int x2 = rnd(x1, 1001);
        std::vector<Point> got;
        for (auto p : rt.range(y1, y2, x1, x2)) got.push_back(p);
        std::sort(got.begin(), got.end(), [](const Point& a, const Point& b){ return a.y < b.y || (a.y==b.y && a.x<b.x); });
        std::vector<Point> exp;
        for (auto &p : base){
            if (p.y >= y1 && p.y < y2 && p.x >= x1 && p.x < x2) exp.push_back(p);
        }
        REQUIRE(got.size() == exp.size());
        for (size_t i=0;i<exp.size();++i){
            REQUIRE(got[i].x == exp[i].x);
            REQUIRE(got[i].y == exp[i].y);
        }
    };

    const int ops = 5000;
    for (int i=1;i<=ops;++i){
        int t = rnd(0, 99);
        if (t < 60 || base.empty()){
            // insert
            Point p{ rnd(0,1000), rnd(0,1000) };
            if (!base.count(p)){
                base.insert(p);
                rt.insert(p, p);
            }
        } else if (t < 90) {
            // remove existing
            int k = rnd(0, (int)base.size()-1);
            auto it = base.begin();
            std::advance(it, k);
            Point p = *it;
            base.erase(it);
            rt.remove(p, p);
        } else {
            // do a query burst
            for (int q=0;q<5;++q) check_once();
        }
        if (i % 50 == 0) check_once();
    }

    // final checks
    for (int q=0;q<20;++q) check_once();
}

// Simple compile-time construction test for 3D default alias
TEST_CASE("RangeTree ND alias build (3D)") {
    using RT3 = ds::RangeTreeND<int,int,int>; // 3D: y,z as outer layers, x as leaf set
    RT3 *ptr = nullptr;
    REQUIRE(ptr == nullptr);
}

TEST_CASE("RangeTreeNDPoints basic 3D usage"){
    using RTP = ds::RangeTreeNDPointsT<int,int,int>;
    RTP rt;
    rt.insert(1,2,3);
    rt.insert(4,5,6);
    std::vector<std::tuple<int,int,int>> v;
    for (auto t : rt.range(1,2, 2,6, 0,10)) v.push_back(t);
    std::sort(v.begin(), v.end());
    REQUIRE(v.size() == 1);
    REQUIRE(v[0] == std::make_tuple(1,2,3));
}

struct P3 { int x,y,z; };
struct CmpZYX {
    bool operator()(const P3& a, const P3& b) const{
        if (a.z != b.z) return a.z < b.z;
        if (a.y != b.y) return a.y < b.y;
        return a.x < b.x;
    }
};

TEST_CASE("RangeTreeND randomized 3D vs naive"){
    using RT3 = ds::RangeTreeND<int,int,int>;
    RT3 rt;
    std::set<P3, CmpZYX> base;
    std::mt19937 rng(987654321);
    auto rnd = [&](int lo, int hi){ std::uniform_int_distribution<int> d(lo,hi); return d(rng); };

    auto check_once = [&](){
        int z1 = rnd(0, 1000); int z2 = rnd(z1, 1001);
        int y1 = rnd(0, 1000); int y2 = rnd(y1, 1001);
        int x1 = rnd(0, 1000); int x2 = rnd(x1, 1001);
        std::vector<int> got;
        for (int x : rt.range(z1,z2, y1,y2, x1,x2)) got.push_back(x);
        std::sort(got.begin(), got.end());
        std::vector<int> exp;
        for (auto &p : base){
            if (p.z>=z1 && p.z<z2 && p.y>=y1 && p.y<y2 && p.x>=x1 && p.x<x2) exp.push_back(p.x);
        }
        std::sort(exp.begin(), exp.end());
        REQUIRE(got.size() == exp.size());
        for (size_t i=0;i<exp.size();++i) REQUIRE(got[i]==exp[i]);
    };

    const int ops = 4000;
    for (int i=1;i<=ops;++i){
        int t = rnd(0,99);
        if (t < 60 || base.empty()){
            P3 p{ rnd(0,1000), rnd(0,1000), rnd(0,1000) };
            if (!base.count(p)){
                base.insert(p);
                rt.insert(p.z, p.y, p.x);
            }
        } else if (t < 90) {
            int k = rnd(0, (int)base.size()-1);
            auto it = base.begin();
            std::advance(it, k);
            P3 p = *it;
            base.erase(it);
            rt.remove(p.z, p.y, p.x);
        } else {
            for (int q=0;q<4;++q) check_once();
        }
        if (i % 50 == 0) check_once();
    }
    for (int q=0;q<20;++q) check_once();
}

