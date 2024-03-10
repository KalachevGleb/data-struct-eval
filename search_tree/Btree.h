#ifndef BTREE_H
#define BTREE_H
#include "alloc.h"
#include <iostream>
#include <memory>
#include <cstring>

//#include "commontree.h"
class BTreeBase{//B-tree base for movable keys
protected:
	ResizableAllocRef _nodes, _leaves;
	//number of keys in B-tree node varies between B and 2*B
	unsigned int B, _2B;
	unsigned int key_size, f_size, f_leaf_size, h;
	//bool val_use_ptr,
	bool check_l, check_r;
	//char *mbuf; //size of this buf must be f_size;
	struct BNode{
		BNode *p;
		struct{
			unsigned int pnum : 16;
			unsigned int knum : 15;
			unsigned int is_leaf : 1;
		};
		//        data in internal node: 
		// [pointer to 0th child node][key 1][value 1]
		// [pointer to 1st child node][key 2][value 2]
		//          ...
		// [pointer to (n-1)-th child node][key n][value n]
		// [pointer to n-th child node]
		// ===========================================
		//  data in leaf node has no pointers: 
		// [key 1][value 1]
		// [key 2][value 2]
		//          ...
		// [key n][value n]
		char data[1];
		void setp(BNode* P, int N){ p = P; pnum = N; }
	};
	struct _findres{
		BNode * n;
		struct{
			unsigned int pos : 16;
			unsigned int is_eq : 1;
		};
		_findres()= default;
		_findres(BNode *nn, int cc, bool eq){ n = nn; pos = cc; is_eq = eq ? 1 : 0; }
	};
	void *key_node_ptr(BNode *l, int nk)const{
		return l->data + sizeof(void*)+(f_size * nk);
	}
	void *key_leaf_ptr(BNode *l, int nk)const{
		return l->data + (key_size * nk);
	}
/*	void *val_node(BNode *l, int nk){
		char *vp = l->data + val_offset + sizeof(void*) + (f_size * nk);
		if (val_use_ptr)return *(void**)(vp);
		return vp;
	}
	void *val_leaf(BNode *l, int nk){
		char *vp = l->data + val_offset + (f_leaf_size * nk);
		if (val_use_ptr)return *(void**)(vp);
		return vp;
	}*/
	void *end_leaf_data(BNode *l)const {
		return l->data + key_size*l->knum;
	}
	void *end_node_data(BNode *l)const {
		return l->data + f_size*l->knum + sizeof(void*);
	}
	int node_size()const {
        return offsetof(BNode, data) + f_size*_2B+sizeof(void*);
    }
	int leaf_size()const {
        return offsetof(BNode, data) + key_size*_2B;
    }
	BNode *new_node() {
        auto *r = (BNode*)(_nodes.get_new());
        if (r)r->is_leaf = false;
        return r;
    }
	BNode *new_leaf(){
        auto *r = (BNode*)(_leaves.get_new()); if (r)r->is_leaf = true; return r;
    }
	void free_node(BNode*x){ return _nodes.free(x); }
	void free_leaf(BNode*x){ return _leaves.free(x); }
	//void move_node_key
	BNode *&down(BNode *l, int nk)const{
		return *(BNode**)(l->data + (f_size * nk));
	}
	/* sets value d to the n-th child pointer of l */
	void set_down(BNode *l, int n, BNode *d)const{
		*(BNode**)(l->data + (f_size * n)) = d;
		d->setp(l, n);
	}
	/*function moves the sequence <(end-start) times (key, value)>
	by d places right starting from key with number start.
	If d is negative then function moves the sequence left*/
	void move_leafdata(BNode *l, int start, int end, int d) {
		memcpy(l->data + key_size*(start + d), l->data + key_size*start, (end - start)*key_size);
	}
	/*function moves the sequence <(end-start) times (child pointer, key, value)>
	by d places right starting from child pointer with number start.
	If d is negative then function moves the sequence left*/
	void move_ldata(BNode *l, int start, int end, int d) {
		char *src = l->data;
		for (int i = start; i < end; i++)
			(*(BNode**)(src + i*f_size))->pnum+=d;
		memcpy(l->data + f_size*(start+d), l->data + f_size*start, (end-start)*f_size);
	}
	/*function moves the sequence <(end-start) times (key, value, child pointer)>
	by d places right starting from key with number start.
	 If d is negative then function moves the sequence left*/
	void move_rdata(BNode *l, int start, int end, int d) {
		char *src = l->data;
		for (int i = start+1; i <= end; i++)
			(*(BNode**)(src + i*f_size))->pnum += d;
		memcpy(l->data + sizeof(void*)+f_size*(start + d),
			l->data + sizeof(void*)+f_size*start,
			(end - start)*f_size);
	}
	/* setting correct pointers p for all children with
	numbers from start to end */
	void set_ptrs(BNode *l, int start, int end){
		char *src = l->data;
		for (int i = start; i < end; i++)
			(*(BNode**)(src + i*f_size))->setp(l, i);
	}
	/* setting correct pointers p for all children with
	numbers from start (by default for all) */
	void set_ptrs(BNode *l, int start = 0){ return set_ptrs(l, start, l->knum + 1); }
	
	// the root of B-tree
	BNode *root;

	//inserts pair (k,v) into leaf l at position pos;
	_findres _insert_leaf(BNode *l, void *k, unsigned int pos);
	//balancing procedure for insert
	void _insert(BNode *into, char *buf, unsigned int pos, BNode *y);
	//removes k-th record from leaf l
	_findres _remove_leaf(BNode *l, unsigned int pos);
	//balancing procedure for remove
	void _remove(BNode *n, unsigned int pos);
//	void _init();
	void _setKVSize(size_t ksz, size_t allsz);
	size_t _sz = 0;
public:
	size_t size()const{return _sz;}
	void setB(int b);
	unsigned getB()const{ return this->B; }
	BTreeBase();
	~BTreeBase();

//	template<class T>
	struct _default_cmp{
		template<class T>
		bool less(const T&x, const T&y)const{
			return x<y;
		}
		template<class T>
		int compare(const T& x, const T&y)const{
			return x<y ? -1 : y<x ? 1 : 0;
		}
	};
//	void _clear();
};

class BTreeVBase{//B-tree base for movable keys
protected:
	ResizableAllocRef _nodes, _leaves;
	//number of keys in B-tree node varies between B and 2*B
	unsigned int B, _2B;
	unsigned int h; //tree height
	unsigned int key_size, val_size, val_offset, ptr_offset, val_off0, key_offset;
	//bool val_use_ptr,
	bool check_l, check_r;
	//char *mbuf; //size of this buf must be f_size;
	struct BNode{
		BNode *p;
		struct{
			unsigned int pnum : 16;
			unsigned int knum : 15;
			unsigned int is_leaf : 1;
		};
		//        data in internal node: 
		// [pointer to 0th child node][key 1][value 1]
		// [pointer to 1st child node][key 2][value 2]
		//          ...
		// [pointer to (n-1)-th child node][key n][value n]
		// [pointer to n-th child node]
		// ===========================================
		//  data in leaf node has no pointers: 
		// [key 1][value 1]
		// [key 2][value 2]
		//          ...
		// [key n][value n]
		char data[1];
		void setp(BNode* P, int N){ p = P; pnum = N; }
	};
	struct _findres{
		BNode * n;
		struct{
			unsigned int pos : 16;
			unsigned int is_eq : 1;
		};
		_findres(){}
		_findres(BNode *nn, int cc, bool eq){ n = nn; pos = cc; is_eq = eq ? 1 : 0; }
	};
	void *key_ptr(BNode *l, int nk)const{
		return l->data + (key_size * nk);
	}
	void *val_ptr(BNode *l, int nk){
		return l->data + val_offset + nk*val_size;
	}
	unsigned int node_size()const{ return int(((BNode*)0)->data - (char*)0) + ptr_offset + int((_2B+1)*sizeof(void*)); }
	unsigned int leaf_size()const{ return int(((BNode*)0)->data - (char*)0) + ptr_offset; }
	BNode *new_node(){ BNode *r = (BNode*)(_nodes.get_new()); if (r)r->is_leaf = false; return r; }
	BNode *new_leaf(){ BNode *r = (BNode*)(_leaves.get_new()); if (r)r->is_leaf = true; return r; }
	void free_node(BNode*x){ return _nodes.free(x); }
	void free_leaf(BNode*x){ return _leaves.free(x); }
	//void move_node_key
	BNode **down(BNode *l)const{
		return ((BNode**)(l->data + ptr_offset));
	}
	/* sets value d to the n-th child pointer of l */
	void set_down(BNode *l, int n, BNode *d)const{
		down(l)[n] = d;
		d->setp(l, n);
	}
	void set_kv(BNode *l, int n, void *k, void *v){
		memcpy(key_ptr(l, n), k, key_size);
		memcpy(val_ptr(l, n), v, val_size);
	}
	void move_kv(BNode *dst, int dst_pos, BNode *src, int src_pos){
		memcpy(key_ptr(dst, dst_pos), key_ptr(src,src_pos), key_size);
		memcpy(val_ptr(dst, dst_pos), val_ptr(src, src_pos), val_size);
	}
	/*function moves the sequence <(end-start) times (key, value)>
	by d places right starting from key with number start.
	If d is negative then function moves the sequence left*/
	void move_leafdata(BNode *l, int start, int end, int d){
		memcpy(key_ptr(l, start + d), key_ptr(l, start), (end - start)*key_size);
		memcpy(val_ptr(l, start + d), val_ptr(l, start), (end - start)*val_size);
	}
	/*function moves the sequence <(end-start) times (key, value, child pointer)>
	by d places right starting from key with number start.
	If d is negative then function moves the sequence left*/
	void move_rdata(BNode *l, int start, int end, int d){
		move_leafdata(l, start, end, d);
		BNode** dn = down(l)+1;
		for (int i = start; i < end; i++)
			dn[i]->pnum += d;
		memcpy(dn + start + d, dn + start, (end - start)*sizeof(BNode*));
	}
	/*function moves the sequence (child pointer, <(end-start) times (key, value, child pointer)>)
	by d places right starting from key with number start.
	If d is negative then function moves the sequence left*/
	void move_lrdata(BNode *l, int start, int end, int d){
		move_leafdata(l, start, end, d);
		BNode** dn = down(l);
		for (int i = start; i <= end; i++)
			dn[i]->pnum += d;
		memcpy(dn + start + d, dn + start, (end - start + 1)*sizeof(BNode*));
	}
	/* setting correct pointers p for all children with
	numbers from start to end */
	void set_ptrs(BNode *l, int start, int end){
		BNode** dn = down(l);
		for (int i = start; i < end; i++)
			dn[i]->setp(l, i);
	}
	/* setting correct pointers p for all children with
	numbers from start (by default for all) */
	void set_ptrs(BNode *l, int start = 0){ return set_ptrs(l, start, l->knum + 1); }

	// the root of B-tree
	BNode *root;

	void _balance(BNode *x);
	//inserts pair (k,v) into leaf l at position pos;
	_findres _insert_leaf(BNode *l, void *k, void *v, unsigned int pos);
	//balancing procedure for insert
	void _insert(BNode *into, void *k, void *v, unsigned int pos, BNode *y);
	//removes k-th record from leaf l
	_findres _remove_leaf(BNode *l, unsigned int pos);
	//balancing procedure for remove
	void _remove(BNode *n, unsigned int pos);
	//	void _init();
	void _setKVSize(size_t ksz, size_t allsz, size_t v_off);
	int _sz;
public:
	int size()const{ return _sz; }
	void setB(int b);
	int getB()const{
		return this->B;
	}
	BTreeVBase();
	~BTreeVBase();
	size_t memory_usage()const{
		return _nodes.memory_usage() + _leaves.memory_usage();
	}
};

/* class Cmp must contain function:
 *    int compare(const K&x, const K&y)const;
 * or
 *    static int compare(const K&x, const K&y).
 *
 * To use other type K1 as argument of findT, insertT, removeT
 * must be defined functions
 *    template<class K1>
 *    int compare(const K&x, const K1&y)const;
 * or
 *    template<class K1>
 *    static int compare(const K&x, const K1&y).
 * in the other case each comparation will convert K1 to K.
 * These functions useful when constructor of class K allocates memory,
 * like std::string and it is ineffcient to convert char* to std::string
 * before each find call.
 * K must be a trivially movable type
 */

template<class K, class Cmp = BTreeBase::_default_cmp>
class BTree:public BTreeBase{
    static_assert(std::is_trivially_move_constructible<K>::value, "K must be a trivially movable type");
	typedef BTreeBase::BNode _node;
	Cmp _cmp;
	struct _kmove{
		char s[sizeof(K)];
	};
	struct _x{
		K key;
		void * p;
	};
	static const unsigned int _v_offset = ((char*)(&((_x*)0)->p) - (char*)0);
	static K* key_ptr(_node *l, int n){
		if (l->is_leaf)return (K*)(l->data + n*sizeof(K));
		return (K*)(l->data + sizeof(void*) + n*(sizeof(void*)+_v_offset));
	}
	static _node* down_ptr(_node *l, int n){
		return *(BNode**)(l->data + n*(sizeof(void*)+_v_offset));
	}
	template<class K1>
	_findres _findins(const K1 &key)const{
		_node *n = this->root;
		int c = 0;
		if (!root)return _findres(0, 0, false);
		int b, e, m;
		while (!n->is_leaf){
			//binary search in internal B-tree node
			for (b = 0, e = n->knum; b < e;){
				m = (b + e) >> 1;
				c = _cmp.compare(*(const K*)key_node_ptr(n, m), key);
				if (c < 0)b = m + 1;
				else if(c>0)e = m;
				else return _findres(n, m, true);
			}
			n = down(n, b);
		}
		//now n is leaf;
		//binary search in leaf
		for (b = 0, e = n->knum; b < e;){
			m = (b + e) >> 1;
			c = _cmp.compare(*(const K*)key_leaf_ptr(n, m), key);
			if (c < 0) b = m + 1;
			else if (c > 0) e = m;
			else return _findres(n, m, true);
		}
		return _findres(n, b, false);
	}
	void _delrec(_node *n){
		if (n->is_leaf){
			for (unsigned int i = 0; i < n->knum; i++){
				_destruct((K*)key_leaf_ptr(n, i));
			}
			this->_leaves.free(n);
		}
		else{
			_delrec(down(n, 0));
			for (unsigned int i = 0; i < n->knum; i++){
				_destruct((K*)key_node_ptr(n, i));
				_delrec(down(n, i + 1));
			}
			this->_nodes.free(n);
		}
	}
	void print_rec(_node *n, int rec = 0){
		if (n->is_leaf){
			for (unsigned int i = 0; i < n->knum; i++){
				for (int j = 0; j < rec; j++)std::cout << "    ";
				std::cout << *(K*)key_leaf_ptr(n, i); std::cout << "\n";
			}
		}
		else{
			print_rec(down_ptr(n, 0), rec + 1);
			for (unsigned int i = 0; i < n->knum; i++){
				for (int j = 0; j < rec; j++)std::cout << "    ";
				std::cout << *(K*)key_node_ptr(n, i); std::cout << "\n";
				print_rec(down_ptr(n, i+1), rec + 1);
			}
		}
	}
public:
	void print(){
		if (!this->root)printf("<empty tree>\n");
		else{
			std::cout << "============================\n";
			print_rec(this->root, 1);
			std::cout << "============================\n\n";
		}
	}
	class const_iterator{
		_node *_n;
		unsigned int _pos = 0;
		explicit const_iterator(_findres r){ _n = r.n; _pos = r.pos; }
		void _go_up(){
			while (_pos >= _n->knum && _n->p){
				_pos = _n->pnum;
				_n = _n->p;
			}
		}
		friend class BTree<K,Cmp>;
	public:
		const_iterator(){_n=nullptr;}
		const_iterator& to_start(){
			if (!_n)return *this;
			while (_n->p)_n = _n->p;
			while (!_n->is_leaf)
				_n = down_ptr(_n, 0);
			_pos = 0;
			return *this;
		}
		const_iterator& to_end(){
			if (!_n)return *this;
			while (_n->p)_n = _n->p;
			_pos = _n->knum;
			return *this;
		}

		bool isValid()const{return _n && _pos < _n->knum;}
		const K& key()const{return *key_ptr(_n, _pos);}
		const K* operator->()const{ return key_ptr(_n, _pos); }
		const_iterator& operator++(){
			if (_n->is_leaf){
				if (++_pos < _n->knum)return *this;
				while (_pos >= _n->knum){
					if (!_n->p) return *this;
					_pos = _n->pnum;
					_n = _n->p;
				}
				return *this;
			}
			_n = down_ptr(_n, _pos + 1);
			while (!_n->is_leaf){
				_n = down_ptr(_n, 0);
			}
			_pos = 0;
			return *this;
		}
		const_iterator& operator--(){
			if (_n->is_leaf){
				//if (--_pos < 0)return *this;
				while (!_pos){
					if (!_n->p){ _pos = _n->knum; return *this; } // go to end
					_pos = _n->pnum;
					_n = _n->p;
				}
				_pos--;
				return *this;
			}
			_n = down_ptr(_n, _pos);
			while (!_n->is_leaf){
				_n = down_ptr(_n, _n->knum);
			}
			_pos = _n->knum-1;
			return *this;
		}
		const_iterator operator++(int){
			const_iterator it(*this);
			++*this;
			return it;
		}
		const_iterator operator--(int){
			const_iterator it(*this);
			--*this;
			return it;
		}
		bool operator == (const_iterator it)const{ return _n == it._n && _pos == it._pos; }
		bool operator != (const_iterator it)const{ return _n != it._n || _pos != it._pos; }
	};

	explicit BTree(int b = 0){
		this->_setKVSize(sizeof(K), _v_offset);
		if (!b)b = 16;
		this->setB(b);
	}
	~BTree(){clear();}
	void clear(){
		if(this->root)_delrec(this->root);
		this->root = 0;
		this->_sz = 0;
	}
	template<class K1>
	const_iterator insertT(const K1& key, bool replace = true){
		_findres fr = _findins(key);
		if (fr.is_eq){
			if (replace)
				*(K*)key_ptr(fr.n, fr.pos) = key;
			return fr;
		}
		char s[sizeof(K)];
		K* k = new(s) K(key);
		return this->_insert_leaf(fr.n, k, fr.pos);
	}
	const_iterator erase(const_iterator it){
		if(it.isValid()){
			const_iterator i = it;
			_destruct(key_ptr(i._n, i._pos));
			if (!i._n->is_leaf){//then it._n is leaf
				++it;
				memcpy(key_ptr(i._n, i._pos), key_ptr(it._n, it._pos), sizeof(K));
			}
			return this->_remove_leaf(it._n, it._pos);
		}
		return it;
	}
	template<class K1>
	bool removeT(const K1& key){
		const_iterator n = findT(key);
		return n!=erase(n);
	}
	template<class K1>
	const_iterator findT(const K1 &key)const{
		_findres r = _findins(key);
		if (r.is_eq)return r;
		return end();
	}
	template<class K1>
	const_iterator findRightT(const K1 &key)const{
		const_iterator r = _findins(key);
		r._go_up();
		return r;
	}
	template<class K1>
	const_iterator findLeftT(const K1 &key)const{
		_findres r = _findins(key);
		const_iterator it(r);
		if (r.is_eq)return it;
		return --it;
	}
	const_iterator findRight(const K &key)const{
		return findRightT(key);
	}
	const_iterator findLeft(const K &key)const{
		return findLeftT(key);
	}
	const_iterator find(const K &key) const{
	//	return _find(key);
		return findT(key);
	}
	template<class K1>
	bool containsT(const K1 &key){
		return findT(key).isValid();
	}
	bool contains(const K&key)const{
		return findT(key).isValid();
	}
	const_iterator insert(const K& key, bool replace=true){
		return insertT(key, replace);
	}
/*	template<class K1>
	BTree<K, Cmp>& operator <<= insert(const K& key){
		insertT(key, replace);
		return *this;
	}*/
	bool remove(const K &key){ return removeT(key); }
	BTree<K, Cmp>& operator<<(const K& key){
		insertT(key, true);
		return *this;
	}
	BTree<K, Cmp>& operator-=(const K& key){
		remove(key);
		return *this;
	}
	const_iterator begin()const{
		const_iterator it;
		it._n = this->root;
		return it.to_start();
	}
	const_iterator end()const{
		if (!this->_sz)return const_iterator();
		const_iterator it;
		it._n = this->root; it._pos = this->root->knum;
		return it;
	}
};

template<class K, class V, class Cmp = BTreeBase::_default_cmp>
class BTreeV :public BTreeVBase{
	typedef BTreeVBase::BNode _node;
	Cmp _cmp;
	struct _kmove{
		char s[sizeof(K)];
	};
	struct _vmove{
		char s[sizeof(V)];
	};
	struct _x{
		char c;
		V val;
	};
//	static const unsigned int _v_offset = ((char*)(&((_x*)0)->p) - (char*)0);
/*	static K* key_ptr(_node *l, int n){
		if (l->is_leaf)return (K*)(l->data + n*sizeof(K));
		return (K*)(l->data + sizeof(void*)+n*(sizeof(void*)+_v_offset));
	}*/
/*	static _node* down_ptr(_node *l, int n){
		return *(BNode**)(l->data + n*(sizeof(void*)+_v_offset));
	}*/
	template<class K1>
	_findres _findins(const K1 &key)const{
		_node *n = this->root;
		int c = 0;
		if (!root)return _findres(nullptr, 0, false);
		int b, e, m;
		for(;;){
			//binary search in internal B-tree node
			for (b = 0, e = n->knum; b < e;){
				m = (b + e) >> 1;
				c = _cmp.compare(*(const K*)key_ptr(n, m), key);
				if (c < 0)b = m + 1;
				else if (c>0)e = m;
				else return _findres(n, m, true);
			}
			if (n->is_leaf)break;
			n = down(n)[b];
		}
		return _findres(n, b, false);
	}
	void _delrec(_node *n){
		if (n->is_leaf){
			for (unsigned int i = 0; i < n->knum; i++){
				_destruct((K*)key_ptr(n, i));
				_destruct((V*)val_ptr(n, i));
			}
			this->_leaves.free(n);
		}
		else{
			_delrec(down(n)[0]);
			for (unsigned int i = 0; i < n->knum; i++){
				_destruct((K*)key_ptr(n, i));
				_destruct((V*)val_ptr(n, i));
				_delrec(down(n)[i + 1]);
			}
			this->_nodes.free(n);
		}
	}
	void print_rec(_node *n, int rec = 0){
		if (n->is_leaf){
			for (unsigned int i = 0; i < n->knum; i++){
				for (int j = 0; j < rec; j++)std::cout << "    ";
				std::cout << *(K*)key_ptr(n, i) << "   " << *(V*)val_ptr(n,i); std::cout << "\n";
			}
		}
		else{
			print_rec(down(n)[0], rec + 1);
			for (unsigned int i = 0; i < n->knum; i++){
				for (int j = 0; j < rec; j++)std::cout << "    ";
				std::cout << *(K*)key_ptr(n, i) << "   " << *(V*)val_ptr(n, i); std::cout << "\n";
				print_rec(down(n)[i + 1], rec + 1);
			}
		}
	}
public:
	void print(){
		if (!this->root)printf("<empty tree>\n");
		else{
			std::cout << "============================\n";
			print_rec(this->root, 1);
			std::cout << "============================\n\n";
		}
	}
	class const_iterator{
		const BTreeV<K, V,Cmp> *_t;
		_node *_n;
		unsigned int _pos;
		const_iterator(const BTreeV<K, V, Cmp> *t, _findres r){ _t = t; _n = r.n; _pos = r.pos; }
		void _go_up(){
			while (_pos >= _n->knum && _n->p){
				_pos = _n->pnum;
				_n = _n->p;
			}
		}
		friend class BTreeV<K, V, Cmp>;
	public:
		const_iterator(){ _n = 0; _t = 0; }
		const_iterator& to_start(){
			if (!_n)return *this;
			while (_n->p)_n = _n->p;
			while (!_n->is_leaf)
				_n = _t->down(_n)[0];
			_pos = 0;
			return *this;
		}
		const_iterator& to_end(){
			if (!_n)return *this;
			while (_n->p)_n = _n->p;
			_pos = _n->knum;
			return *this;
		}

		bool isValid()const{ return _n && _pos < _n->knum; }
		const K& key()const{ return *(K*)_t->key_ptr(_n, _pos); }
		const K* operator->()const{ return (K*)_t->key_ptr(_n, _pos); }
		const_iterator& operator++(){
			if (_n->is_leaf){
				if (++_pos < _n->knum)return *this;
				while (_pos >= _n->knum){
					if (!_n->p) return *this;
					_pos = _n->pnum;
					_n = _n->p;
				}
				return *this;
			}
			_n = _t->down(_n)[_pos + 1];
			while (!_n->is_leaf){
				_n = _t->down(_n)[0];
			}
			_pos = 0;
			return *this;
		}
		const_iterator& operator--(){
			if (_n->is_leaf){
				//if (--_pos < 0)return *this;
				while (!_pos){
					if (!_n->p){ _pos = _n->knum; return *this; } // go to end
					_pos = _n->pnum;
					_n = _n->p;
				}
				_pos--;
				return *this;
			}
			_n = _t->down(_n)[_pos];
			while (!_n->is_leaf){
				_n = _t->down(_n)[_n->knum];
			}
			_pos = _n->knum - 1;
			return *this;
		}
		const_iterator operator++(int){
			const_iterator it(*this);
			++*this;
			return it;
		}
		const_iterator operator--(int){
			const_iterator it(*this);
			--*this;
			return it;
		}
		bool operator == (const_iterator it)const{ return _n == it._n && _pos == it._pos; }
		bool operator != (const_iterator it)const{ return _n != it._n || _pos != it._pos; }
	};

	BTreeV(int b = 0){
		this->_setKVSize(sizeof(K), sizeof(V), (char*)(&((_x*)0)->val)-(char*)0);
		if (!b)b = 16;
		this->setB(b);
	}
	~BTreeV(){ clear(); }
	void clear(){
		if (this->root)_delrec(this->root);
		this->root = 0;
		this->_sz = 0;
	}
	template<class K1, class V1>
	const_iterator insertT(const K1& key, const V1 &val, bool replace = true){
		_findres fr = _findins(key);
		if (fr.is_eq){
			if (replace)
				*(K*)key_ptr(fr.n, fr.pos) = key;
			return const_iterator(this,fr);
		}
		char sk[sizeof(K)];
		char sv[sizeof(V)];
		K* k = new(sk)K(key);
		V* v = new(sv)V(val);
		return const_iterator(this, this->_insert_leaf(fr.n, k, v, fr.pos));
	}
	const_iterator erase(const_iterator it){
		if (it.isValid()){
			const_iterator i = it;
			_destruct((K*)key_ptr(i._n, i._pos));
			_destruct((V*)val_ptr(i._n, i._pos));
			if (!i._n->is_leaf){//then it._n is leaf
				++it;
				move_kv(i._n, i._pos, it._n, it._pos);
			}
			return const_iterator(this,this->_remove_leaf(it._n, it._pos));
		}
		return it;
	}
	template<class K1>
	bool removeT(const K1& key){
		const_iterator n = findT(key);
		return n != erase(n);
	}
	template<class K1>
	const_iterator findT(const K1 &key)const{
		_findres r = _findins(key);
		if (r.is_eq)return const_iterator(this, r);
		return end();
	}
	template<class K1>
	const_iterator findRightT(const K1 &key)const{
		const_iterator r(this, _findins(key));
		r._go_up();
		return r;
	}
	template<class K1>
	const_iterator findLeftT(const K1 &key)const{
		_findres r = _findins(key);
		const_iterator it(r);
		if (r.is_eq)return it;
		return --it;
	}
	const_iterator findRight(const K &key)const{
		return findRightT(key);
	}
	const_iterator findLeft(const K &key)const{
		return findLeftT(key);
	}
	const_iterator find(const K &key) const{
		//	return _find(key);
		return findT(key);
	}
	template<class K1>
	bool containsT(const K1 &key){
		return findT(key).isValid();
	}
	bool contains(const K&key)const{
		return findT(key).isValid();
	}
	const_iterator insert(const K& key, const V& val, bool replace = true){
		return insertT(key, val, replace);
	}
	/*	template<class K1>
	BTree<K, Cmp>& operator <<= insert(const K& key){
	insertT(key, replace);
	return *this;
	}*/
	bool remove(const K &key){ return removeT(key); }
/*	BTree<K, Cmp>& operator<<(const K& key){
		insertT(key, replace);
		return *this;
	}
	BTree<K, Cmp>& operator-=(const K& key){
		remove(key);
		return *this;
	}*/
	const_iterator begin()const{
		const_iterator it;
		it._n = this->root;
		return it.to_start();
	}
	const_iterator end()const{
		if (!this->_sz)return const_iterator();
		const_iterator it;
		it._n = this->root; it._pos = this->root->knum; it._t = this;
		return it;
	}
};

#endif // BTREE_H
