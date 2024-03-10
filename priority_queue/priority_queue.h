#pragma once
#include <vector>

using namespace std;
class PriorityQueue {
	struct ListNode {
		ListNode *prev, *next;
		int w, n;
		ListNode() {
			w = n = -1;
			prev = next = this;
		}
		ListNode(ListNode &&n) {
			*this = std::forward<ListNode>(n);
		}
		ListNode& operator=(ListNode &&n) {
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
		void remove() {
			prev->next = next;
			next->prev = prev;
		}
		void del() { // delete from current list
			remove();
			next = 0;
		}
		void insert_in_list(ListNode &head) {
			next = head.next;
			prev = &head;
			head.next = next->prev = this;
		}
		bool in_list()const { return next != 0; }
		bool is_empty()const { return next == this; }
		ListNode(const ListNode &n) {
			this->n = n.n;
			w = n.w;
			prev = n.prev;
			next = n.next;
		}
	private:
	};
	int _size, _maxval, _minval;
	vector<ListNode> nodes;        // memory for list nodes 
	vector<ListNode> lists;   // array or cyclic linked lists
public:
	class ListIterator {
		const ListNode *n;
		friend class PriorityQueue;
	public:
		ListIterator(const ListNode *node=0) { n = node; }
		ListIterator& operator++() { n = n->next; return *this; }
		bool isValid()const { return n->w>=0; }
		int operator*()const { return n->n; }
		int w()const { return n->w; }
	};
	class Element {
		mutable ListNode *node;
		mutable PriorityQueue *qu;
		Element(PriorityQueue*q, ListNode *n) :qu(q), node(n) {}
		friend class PriorityQueue;
	public:
		Element() { node = 0; qu = 0; }
		const Element& operator+=(int delta)const {
			if (!delta)return *this;
			node->w += delta;
			node->remove();
			if (delta > 0) {
				if (node->w > qu->_maxval) {
					if (qu->lists.size() <= size_t(node->w))
						qu->lists.resize(node->w + 1);
					qu->_maxval = node->w;
				}
			} else if (node->w < qu->_minval)
				qu->_minval = node->w;
			node->insert_in_list(qu->lists[node->w]);
			if (delta > 0) {
				while (qu->lists[qu->_minval].is_empty())
					qu->_minval++;
			} else {
				while (qu->lists[qu->_maxval].is_empty())
					qu->_maxval--;
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
		_size = sz;
		_maxval = _minval = 0;
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
		return Element(const_cast<PriorityQueue*>(this), const_cast<ListNode*>(&nodes[i]));
	}
	int w(int i)const { return nodes[i].w; }
	ListIterator getMaxIterator() { return ListIterator(lists[_maxval].next); }
	ListIterator getMinIterator() { return ListIterator(lists[_minval].next); }
	ListIterator getValIterator(int i) { return lists[i].next; }
	int minW()const { return _minval; }
	int maxW()const { return _maxval; }
	void moveHead(int i, ListIterator it) {
		if (!it.isValid())return;
		ListNode *h = &lists[i], *n = (ListNode*)it.n, *p = n->prev;
		if (n == h || p == h)return;
		h->next->prev = h->prev;
		h->prev->next = h->next;
		h->next = n;
		h->prev = p;
		n->prev = p->next = h;
	}
	//typedef vector<ListNode>::const_iterator iterator;
	//iterator begin() {return }
};
