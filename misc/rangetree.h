#pragma once

#include <cstdint>
#include <random>
#include <utility>
#include <type_traits>
#include <algorithm>
#include <vector>
#include <iterator>
#include <tuple>
#include <memory>

// RangeTree: a 2D range tree over keys of type K with lower-level structure Sub.
// Template parameters:
//  - K: key type
//  - B: boundary type for the first dimension (for [b1; b2) queries)
//  - Cmp: comparator providing operator()(const K&, const K&) and operator()(const K&, const B&)
//         Both operator() must behave as strict-weak-order "less" predicates.
//  - Sub: lower-level structure type. It is expected to support at least:
//         - insert(const X&)
//         - remove(const X&)
//         and any query API the user will use on the aggregated Sub instances.
//
// Supported operations:
//  - insert(k, x): inserts a value x (the payload accepted by Sub) under the first-dimension key k
//  - remove(k, x): removes a value x under the key k (if present)
//  - find_key(k): returns pointer to per-key Sub for modifications/queries of that specific key
//  - visit_cover(b1, b2, fn): calls fn(const Sub&) for O(log n) canonical aggregated blocks that
//    together cover keys k in [b1; b2). The caller can perform its own second-dimension query on
//    each aggregated Sub to answer [b1; b2) x X in total O(C * log n), where C is Sub's query cost.
//
// Notes:
//  - The tree is implemented as a treap (randomized balanced BST). Expected O(log n) height.
//  - Each node stores:
//      - self: Sub for values with exactly this key
//      - agg: Sub aggregating values across the entire subtree of this node
//    Aggregates are maintained incrementally along the root->key path on insert/remove.

namespace ds {

template <class K, class B, class Cmp, class Sub>
class RangeTree {
public:
    struct Node{
        K key;
        uint32_t pri;
        Node* l;
        Node* r;
        Sub self; // values having exactly this key
        Sub agg;  // aggregate for entire subtree rooted at this node

        explicit Node(const K& k, uint32_t p): key(k), pri(p), l(nullptr), r(nullptr), self(), agg() {}
    };
    // A forward range that concatenates multiple per-Sub ranges of the same type R
    template <class R>
    class ConcatRange {
    public:
        using RangeType = R;
        using InnerIt = decltype(std::declval<R&>().begin());
        using value_type = typename std::iterator_traits<InnerIt>::value_type;
        using reference = decltype(*std::declval<InnerIt&>());
        using const_reference = reference;

        explicit ConcatRange(std::vector<R> parts): parts_(std::move(parts)) {}

        class iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = typename ConcatRange::value_type;
            using difference_type = std::ptrdiff_t;
            using reference = typename ConcatRange::reference;
            using pointer = void;

            iterator(): owner_(nullptr), idx_(0) {}
            explicit iterator(const ConcatRange* owner, bool is_end=false): owner_(owner) {
                if (!owner_){ idx_ = 0; return; }
                if (is_end){ idx_ = owner_->parts_.size(); return; }
                idx_ = 0;
                if (!owner_->parts_.empty()){
                    it_ = owner_->parts_[0].begin();
                    it_end_ = owner_->parts_[0].end();
                    advance_to_valid();
                }
            }
            reference operator*() const { return *it_; }
            iterator& operator++(){ ++it_; advance_to_valid(); return *this; }
            bool operator==(const iterator& other) const{
                if (owner_ != other.owner_) return false;
                if (idx_ == owner_->parts_.size() && other.idx_ == other.owner_->parts_.size()) return true;
                return idx_ == other.idx_ && it_ == other.it_;
            }
            bool operator!=(const iterator& other) const{ return !(*this == other); }
        private:
            void advance_to_valid(){
                while (owner_ && idx_ < owner_->parts_.size() && it_ == it_end_){
                    ++idx_;
                    if (idx_ < owner_->parts_.size()){
                        it_ = owner_->parts_[idx_].begin();
                        it_end_ = owner_->parts_[idx_].end();
                    }
                }
            }
            const ConcatRange* owner_;
            std::size_t idx_ = 0;
            InnerIt it_{};
            InnerIt it_end_{};
        };

        iterator begin() const { return iterator(this, false); }
        iterator end() const { return iterator(this, true); }
        bool empty() const { return parts_.empty(); }
        std::size_t size() const { return parts_.size(); }
    private:
        std::vector<R> parts_;
    };

    // A fully lazy forward range over elements inside keys [b1;b2), no dynamic allocations.
    template <class... Xs>
    class LazyRange {
    public:
        class iterator {
        public:
            using R = decltype(std::declval<const Sub&>().range(std::declval<const Xs&>()...));
            using InnerIt = decltype(std::declval<R&>().begin());
            using difference_type = std::ptrdiff_t;
            using reference = decltype(*std::declval<InnerIt&>());
            using value_type = std::remove_cv_t<std::remove_reference_t<reference>>;
            using iterator_category = std::forward_iterator_tag;
            using pointer = void*;

            iterator(): owner_(nullptr), ended_(true), top_(0) {}
            explicit iterator(const LazyRange* owner, bool is_end): owner_(owner), ended_(is_end), top_(0) {
                if (!ended_) init_begin();
            }
            reference operator*() const { return *it_; }
            iterator& operator++(){
                ++it_;
                if (it_ == it_end_) advance_node();
                return *this;
            }
            bool operator==(const iterator& o) const{
                if (ended_ && o.ended_) return true;
                return ended_ == o.ended_ && owner_ == o.owner_ && it_ == o.it_ && top_ == o.top_;
            }
            bool operator!=(const iterator& o) const { return !(*this == o); }
        private:
            static constexpr int kMaxH = 128;
            const LazyRange* owner_;
            bool ended_;
            const Node* stack_[kMaxH];
            int top_;
            std::unique_ptr<R> cur_range_;
            InnerIt it_{};
            InnerIt it_end_{};

            void init_begin(){
                push_left(owner_->root_);
                advance_node();
            }
            void push_left(const Node* n){
                while (n){
                    if (owner_->owner_->less_k_b(n->key, owner_->b1_)){
                        n = n->r;
                    } else if (!owner_->owner_->less_k_b(n->key, owner_->b2_)){
                        n = n->l;
                    } else {
                        if (top_ < kMaxH) stack_[top_++] = n; // capped; expected log n
                        n = n->l;
                    }
                }
            }
            template<class Tuple, std::size_t... I>
            R make_range_impl(const Sub& s, const Tuple& t, std::index_sequence<I...>) const{
                return s.range(std::get<I>(t)...);
            }
            R make_range(const Sub& s) const{
                return make_range_impl(s, owner_->xs_, std::index_sequence_for<Xs...>{});
            }
            void advance_node(){
                while (top_ > 0){
                    const Node* n = stack_[--top_];
                    // setup current range
                    cur_range_.reset(new R(make_range(n->self)));
                    it_ = cur_range_->begin();
                    it_end_ = cur_range_->end();
                    // prepare right subtree for future
                    push_left(n->r);
                    if (it_ != it_end_) return;
                }
                ended_ = true;
            }
        };

        LazyRange(const RangeTree* owner, const B& b1, const B& b2, const Xs&... xs)
        : owner_(owner), root_(owner->root_), b1_(b1), b2_(b2), xs_(xs...){ }

        iterator begin() const { return iterator(this, false); }
        iterator end() const { return iterator(this, true); }
    private:
        const RangeTree* owner_;
        const Node* root_;
        B b1_;
        B b2_;
        std::tuple<std::decay_t<Xs>...> xs_;
    };

    RangeTree(): rng_(random_seed_()) {}
    explicit RangeTree(Cmp cmp): cmp_(std::move(cmp)), rng_(random_seed_()) {}
    RangeTree(const RangeTree& other): cmp_(other.cmp_), rng_(random_seed_()) {
        root_ = clone_nodes(other.root_);
    }
    RangeTree& operator=(const RangeTree& other){
        if (this == &other) return *this;
        clear();
        cmp_ = other.cmp_;
        root_ = clone_nodes(other.root_);
        return *this;
    }
    RangeTree(RangeTree&& other) noexcept: cmp_(std::move(other.cmp_)), root_(other.root_), rng_(random_seed_()) {
        other.root_ = nullptr;
    }
    RangeTree& operator=(RangeTree&& other) noexcept{
        if (this == &other) return *this;
        clear();
        cmp_ = std::move(other.cmp_);
        root_ = other.root_;
        other.root_ = nullptr;
        return *this;
    }
    ~RangeTree(){ clear(); }

    void clear(){
        destroy_nodes(root_);
        root_ = nullptr;
    }

    bool empty() const { return root_ == nullptr; }

    // Inserts value x under key k.
    template <class... Xs>
    void insert(const K& k, const Xs&... xs){
        Node* n = ensure_key_node(k);
        n->self.insert(xs...);
        insert_agg_along_path(k, xs...);
    }

    // Removes value x under key k. If key absent or value not found, this is a no-op.
    template <class... Xs>
    void remove(const K& k, const Xs&... xs){
        const int kMaxH = 64;
        Node* path[kMaxH];
        int ph = 0;
        Node* cur = root_;
        while (cur){
            if (ph < kMaxH) path[ph] = cur;
            ++ph;
            if (less_k_k(k, cur->key)){
                cur = cur->l;
            } else if (less_k_k(cur->key, k)){
                cur = cur->r;
            } else {
                for (int i = 0; i < std::min(ph, kMaxH); ++i) path[i]->agg.remove(xs...);
                cur->self.remove(xs...);
                maybe_erase_if_empty(cur->key);
                return;
            }
        }
    }

    // Returns pointer to the Sub for a specific key if present; nullptr otherwise.
    Sub* find_key(const K& k){
        Node* n = root_;
        while (n){
            if (less_k_k(k, n->key)) n = n->l;
            else if (less_k_k(n->key, k)) n = n->r;
            else return &n->self;
        }
        return nullptr;
    }
    const Sub* find_key(const K& k) const{
        const Node* n = root_;
        while (n){
            if (less_k_k(k, n->key)) n = n->l;
            else if (less_k_k(n->key, k)) n = n->r;
            else return &n->self;
        }
        return nullptr;
    }

    // Visits O(log n) aggregated Sub blocks fully covering keys in [b1; b2).
    // fn signature: void fn(const Sub& agg) const
    template <class Fn>
    void visit_cover(const B& b1, const B& b2, Fn&& fn) const{
        if (!root_) return;
        // find split node s where paths to b1 and b2 diverge
        const Node* s = root_;
        while (s){
            if (less_k_b(s->key, b1)) s = s->r;          // s->key < b1
            else if (!less_k_b(s->key, b2)) s = s->l;    // s->key >= b2
            else break;                                  // b1 <= s->key < b2
        }
        if (!s) return;

        // left side: from s->l downwards
        const Node* v = s->l;
        while (v){
            if (less_k_b(v->key, b1)){
                v = v->r;
            } else {
                if (v->r) fn(v->r->agg); // right subtree is fully within [b1;b2)
                fn(v->self);             // this key is within [b1;b2)
                v = v->l;
            }
        }
        // include split node's key itself
        fn(s->self);
        // right side: from s->r downwards
        v = s->r;
        while (v){
            if (!less_k_b(v->key, b2)){
                v = v->l;
            } else {
                if (v->l) fn(v->l->agg); // left subtree is fully within [b1;b2)
                fn(v->self);             // this key is within [b1;b2)
                v = v->r;
            }
        }
    }

    // Generic 2D range query dispatcher.
    // Calls cb(sub, x) for each canonical aggregated block 'sub' covering [b1;b2).
    // Users can pass a callback that queries Sub using its own API on the given X.
    template <class X, class CB>
    void range_query(const B& b1, const B& b2, const X& x, CB&& cb) const{
        visit_cover(b1, b2, [&](const Sub& s){ cb(s, x); });
    }

    // Return an iterable concatenation of per-Sub ranges covering keys in [b1;b2).
    // Sub must provide: auto range(const Xs&...) const -> R, where R has begin()/end().
    template <class... Xs>
    auto range(const B& b1, const B& b2, const Xs&... xs) const {
        return LazyRange<Xs...>(this, b1, b2, xs...);
    }

private:
    
    Node* root_ = nullptr;
    Cmp cmp_{};
    std::mt19937 rng_;

    static std::mt19937::result_type random_seed_(){
        std::random_device rd;
        return rd();
    }

    bool less_k_k(const K& a, const K& b) const { return cmp_(a, b); }
    bool less_k_b(const K& a, const B& b) const { return cmp_(a, b); }

    template <class R, class... Xs>
    void collect_key_ranges(const Node* n, const B& b1, const B& b2, std::vector<R>& out, const Xs&... xs) const{
        if (!n) return;
        if (less_k_b(n->key, b1)){
            collect_key_ranges<R>(n->r, b1, b2, out, xs...);
            return;
        }
        if (!less_k_b(n->key, b2)){
            collect_key_ranges<R>(n->l, b1, b2, out, xs...);
            return;
        }
        // b1 <= n->key < b2: include this key and both sides recursively
        collect_key_ranges<R>(n->l, b1, b2, out, xs...);
        out.emplace_back(n->self.range(xs...));
        collect_key_ranges<R>(n->r, b1, b2, out, xs...);
    }

    // Treap rotations via split/merge operations
    static void split_by_key(Node* t, const K& k, const Cmp& cmp, Node*& L, Node*& R){
        // L: keys < k, R: keys >= k
        if (!t){ L = R = nullptr; return; }
        if (cmp(t->key, k)){
            // t->key < k
            split_by_key(t->r, k, cmp, t->r, R);
            L = t;
        } else {
            split_by_key(t->l, k, cmp, L, t->l);
            R = t;
        }
    }
    static Node* merge(Node* L, Node* R){
        if (!L) return R;
        if (!R) return L;
        if (L->pri < R->pri){
            L->r = merge(L->r, R);
            return L;
        } else {
            R->l = merge(L, R->l);
            return R;
        }
    }

    Node* ensure_key_node(const K& k){
        // First, attempt to find existing node quickly
        Node* cur = root_;
        Node* parent = nullptr;
        while (cur){
            parent = cur;
            if (less_k_k(k, cur->key)) cur = cur->l;
            else if (less_k_k(cur->key, k)) cur = cur->r;
            else return parent; // found
        }
        // Not found: insert new node using treap insert with split
        Node *L = nullptr, *R = nullptr;
        split_by_key(root_, k, cmp_, L, R);
        Node* nn = new Node(k, rng_());
        // aggregate for a new key initially empty (self, agg are default)
        // for an empty node, agg must reflect self, but we maintain agg incrementally on inserts/removes
        root_ = merge(merge(L, nn), R);
        return nn; // Return pointer valid in current tree structure
    }

    void destroy_nodes(Node* n){
        if (!n) return;
        destroy_nodes(n->l);
        destroy_nodes(n->r);
        delete n;
    }

    Node* clone_nodes(Node* n){
        if (!n) return nullptr;
        Node* m = new Node(n->key, n->pri);
        m->self = n->self;
        m->agg = n->agg;
        m->l = clone_nodes(n->l);
        m->r = clone_nodes(n->r);
        return m;
    }

    template <class... Xs>
    void insert_agg_along_path(const K& k, const Xs&... xs){
        Node* cur = root_;
        while (cur){
            cur->agg.insert(xs...);
            if (less_k_k(k, cur->key)) cur = cur->l;
            else if (less_k_k(cur->key, k)) cur = cur->r;
            else break;
        }
    }

    // Erase a node by key if its per-key Sub is empty. Detected via ADL/SFINAE for Sub::empty().
    void maybe_erase_if_empty(const K& k){
        if constexpr (has_empty<Sub>::value){
            Node* cur = root_;
            Node* parent = nullptr;
            bool went_left = false;
            while (cur){
                if (less_k_k(k, cur->key)) parent = cur, went_left = true, cur = cur->l;
                else if (less_k_k(cur->key, k)) parent = cur, went_left = false, cur = cur->r;
                else break;
            }
            if (!cur) return;
            if (!cur->self.empty()) return;
            // remove cur from treap (merge its children)
            Node* merged = merge(cur->l, cur->r);
            if (!parent){
                delete cur;
                root_ = merged;
            } else {
                if (went_left) parent->l = merged; else parent->r = merged;
                delete cur;
            }
        }
    }

    template <class T>
    struct has_empty {
        template <class U>
        static auto test(int) -> decltype(std::declval<const U&>().empty(), std::true_type{});
        template <class>
        static std::false_type test(...);
        static constexpr bool value = decltype(test<T>(0))::value;
    };
};

} // namespace ds 