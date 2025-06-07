#pragma once
#include <vector>

using namespace std;
class PriorityQueue {
	struct Node {
		Node *prev, *next;
		int w, n;
		Node() {
			w = n = -1;
			prev = next = this;
		}
		Node(Node &&n) {
			*this = std::forward<Node>(n);
		}
		Node& operator=(Node &&n) {
			if (n.next == &n)
				prev = next = this;
			else {
				prev = n.prev, next = n.next;
				next->prev = prev->next = this;
			}
			w = n.w;
			this->n = n.n;
			return *this;
		}
		void remove_from_list() {
			prev->next = next;
			next->prev = prev;
		}
		void detach() { // delete from current list
			remove_from_list();
			next = 0;
		}
		void insert_in_list(Node &head) {
			next = head.next;
			prev = &head;
			head.next = next->prev = this;
		}
		bool in_list()const { return next != 0; }
		bool is_empty()const { return next == this; }
		Node(const Node &n) {
			this->n = n.n;
			w = n.w;
			prev = n.prev;
			next = n.next;
		}
	private:
	};
	int size_, max_val_, min_val_;
	vector<Node> nodes;        // memory for list nodes 
	vector<Node> lists;   // array or cyclic linked lists
public:
	class ListIterator {
		const Node *n;
		friend class PriorityQueue;
	public:
		ListIterator(const Node *node=0) { n = node; }
		ListIterator& operator++() { n = n->next; return *this; }
		bool isValid()const { return n->w>=0; }
		int operator*()const { return n->n; }
		int weight()const { return n->w; }
	};
	class Element {
		mutable Node *node;
		mutable PriorityQueue *qu;
		Element(PriorityQueue*q, Node *n) :qu(q), node(n) {}
		friend class PriorityQueue;
	public:
		Element() { node = 0; qu = 0; }
		const Element& operator+=(int delta)const {
			if (!delta)return *this;
			node->w += delta;
			node->remove_from_list();
			if (delta > 0) {
				if (node->w > qu->max_val_) {
					if (qu->lists.size() <= size_t(node->w))
						qu->lists.resize(node->w + 1);
					qu->max_val_ = node->w;
				}
			} else if (node->w < qu->min_val_)
				qu->min_val_ = node->w;
			node->insert_in_list(qu->lists[node->w]);
			if (delta > 0) {
				while (qu->lists[qu->min_val_].is_empty())
					qu->min_val_++;
			} else {
				while (qu->lists[qu->max_val_].is_empty())
					qu->max_val_--;
			}
			return *this;
		}
		const Element& operator-=(int delta)const {
			return operator+=(-delta);
		}
		const Element& operator++()const {
			return operator+=(1);
		}
		const Element& operator--()const {
			return operator+=(-1);
		}
		const Element& operator=(int i)const {
			return operator+=(i - node->w);
		}
		operator int()const {
			return node->w;
		}
	};
	class ConstElement : Element {
		friend class PriorityQueue;
	public:
		ConstElement(const Element&x) :Element(x) {}
		ConstElement(Element&& x) :Element(std::move(x)) {}
		ConstElement() {}
		operator int()const {
			return node->w;
		}
	};
	void init(int sz, int maxval) {
		size_ = sz;
		max_val_ = min_val_ = 0;
		nodes.resize(sz);
		lists.resize(maxval+1);
		for (int i = 0; i < sz; i++) {
			nodes[i].w = 0;
			nodes[i].n = i;
			nodes[i].insert_in_list(lists[0]);
		}
	}
	Element operator[](int i) {
		return Element(this, nodes.data() + i);
	}
	ConstElement operator[](int i)const {
		return Element(const_cast<PriorityQueue*>(this), const_cast<Node*>(&nodes[i]));
	}
	int w(int i)const { return nodes[i].w; }
	ListIterator max_iterator() { return ListIterator(lists[max_val_].next); }
	ListIterator min_iterator() { return ListIterator(lists[min_val_].next); }
	ListIterator value_iterator(int i) { return lists[i].next; }
	int min_weight()const { return min_val_; }
	int max_weight()const { return max_val_; }
	void move_head(int i, ListIterator it) {
		if (!it.isValid())return;
		Node *h = &lists[i], *n = (Node*)it.n, *p = n->prev;
		if (n == h || p == h)return;
		h->next->prev = h->prev;
		h->prev->next = h->next;
		h->next = n;
		h->prev = p;
		n->prev = p->next = h;
	}
	//typedef vector<Node>::const_iterator iterator;
	//iterator begin() {return }
};
