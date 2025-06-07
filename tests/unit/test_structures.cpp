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

