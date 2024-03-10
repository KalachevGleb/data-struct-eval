#ifndef AVLTREE_H
#define AVLTREE_H
//#define USE_NATIVE_POINTERS

#include "alloc.h"
#include "commontree.h"
class AVLTreeBase{
protected:
	struct AVLNodeBase{
		/*		int key, kr;
				int rmost;*/
#if defined(_WIN64) && !defined(USE_NATIVE_POINTERS)
		union{
			unsigned char _b[16];
			struct{
				unsigned long long L, H;
			};
		};
		AVLNodeBase *setL(AVLNodeBase *ll){
			L = (L & 0xFFFFFF0000000000ll) | ((char*)ll - (char*)0);
			return ll;
		}
		AVLNodeBase *setR(AVLNodeBase *ll){
			L = (L & 0xFFFFFFFFFFll) | (((char*)ll - (char*)0) << 40);
			H = (H & 0xFFFFFFFFFFFF0000ll) | (((char*)ll - (char*)0)>>24);
			return ll;
		}
		AVLNodeBase *setP(AVLNodeBase *ll){
			H = (H & 0xFF0000000000FFFFll) | (((char*)ll - (char*)0) << 16);
			return ll;
		}
		AVLNodeBase *left(){ return (AVLNodeBase*)((char*)0+(L&0xFFFFFFFFFFll)); }
		AVLNodeBase *right(){ return (AVLNodeBase*)((char*)0 + ((L>>40) | ((H&0xFFFFll)<<24))); }
		AVLNodeBase *up(){ return (AVLNodeBase*)((char*)0 + ((H>>16)&0xFFFFFFFFFFll)); }
		AVLNodeBase *next(){ return nextNodeF(this); }
		AVLNodeBase *prev(){ return prevNodeF(this); }
		void setH(int h){ _b[15] = h; }
		unsigned char height()const{ return _b[15]; }
		unsigned char& height(){ return _b[15]; }
		bool isNil()const{ return !_b[15]; }
		AVLNodeBase(){_b[15]=1;}
#else
		AVLNodeBase *l;
		AVLNodeBase *r;
		AVLNodeBase *p;
		unsigned char h;
		AVLNodeBase *setL(AVLNodeBase *ll){ return l = ll; }
		AVLNodeBase *setR(AVLNodeBase *ll){ return r = ll; }
		AVLNodeBase *setP(AVLNodeBase *ll){ return p = ll; }
		AVLNodeBase *left(){ return l; }
		AVLNodeBase *right(){ return r; }
		AVLNodeBase *up(){ return p; }
		const AVLNodeBase *next()const { return nextNode(this); }
		const AVLNodeBase *prev()const { return prevNode(this); }
        AVLNodeBase *next() { return nextNode(this); }
        AVLNodeBase *prev() { return prevNode(this); }
		void setH(int hh){ h = hh; }
		unsigned char& height(){ return h; }
		unsigned char height()const{ return h; }
		bool isNil()const{ return !h; }
		AVLNodeBase(){ h = 1; }
#endif
	};
	AVLNodeBase nil;
	void _balance(AVLNodeBase *x);
	void _insert(AVLNodeBase *before, AVLNodeBase *n, bool l);
	void _remove(AVLNodeBase *n);
	void _init();
	int sz;
public:
	int size()const{return sz;}
	AVLTreeBase();
	~AVLTreeBase(){}

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
 */

template<class K, class V, class Cmp = AVLTreeBase::_default_cmp>
class AVLTree:public AVLTreeBase{
	struct AVLNode:public AVLNodeBase{
		K key;
		V val;
		AVLNode(){}
		AVLNode(const K &k, const V &v){
			key = k; val = v;
		}
	};
	Cmp _cmp;
	AllocRef<AVLNode> _al;

	void _delrec(AVLNodeBase *n){
		for(AVLNodeBase *nn = n; nn!=&this->nil; nn = n){
			n = nn->left();
			_delrec(nn->right());
			_al.free((AVLNode*)nn);
		}
	}
public:
	class const_iterator{
		AVLNode *_n;
		const_iterator(const AVLNodeBase *n){_n=(AVLNode*)n;}
		friend class AVLTree<K,V,Cmp>;
	public:
		const_iterator(){_n=0;}
		bool isValid()const{return _n && _n->height();}
		const V& value()const{return _n->val;}
		const K& key()const{return _n->key;}
		const V* operator->()const{return &_n->val;}
		const V& operator*()const{return _n->val;}
		const_iterator& operator++(){
			_n = (AVLNode*)_n->next;
			return *this;
		}
		const_iterator& operator--(){
			_n = (AVLNode*)_n->prev;
			return *this;
		}
		const_iterator operator++(int){
			AVLNode* old = _n;
			_n = (AVLNode*)_n->next;
			return old;
		}
		const_iterator operator--(int){
			AVLNode* old = _n;
			_n = (AVLNode*)_n->prev;
			return old;
		}
		bool operator == (const_iterator it)const{return _n==it._n;}
		bool operator != (const_iterator it)const{return _n!=it._n;}
	};

	AVLTree(){}
	~AVLTree(){clear();}
	void clear(){
		_delrec(this->nil.left());
		this->nil.setL(&this->nil);
		this->nil.setR(&this->nil); 
		this->nil.setP(&this->nil);
		this->sz = 0;
	}
	size_t memory_usage()const{
		return _al.memory_usage();
	}
	template<class K1, class V1>
	const_iterator insertT(const K1& key, const V1 &val, bool replace=true){
		InsertPlaceResult<AVLNode> fr = findInsertPlaceF(key, (AVLNode*)&this->nil, _cmp);
		//AVLNode *pn = (AVLNode*)_findins(key);
		if(fr.n->height() && !fr.c){
			if(replace)(fr.n)->val = val;
			return fr.n;
		}
		AVLNode *newnode = _al.get_new(key, val);
		this->_insert(fr.n, newnode, fr.c>=0);
		return newnode;
	}
	const_iterator erase(const_iterator it){
		if(it.isValid()){
			AVLNodeBase *n = it._n->next();
			this->_remove(it._n);
			_al.free(it._n);
		//	this->sz--;
			return n;
		}return it;
	}
	template<class K1>
	bool removeT(const K1& key){
		const_iterator n = findT(key);
		return n!=erase(n);
	}
	template<class K1>
	const_iterator findT(const K1 &key)const{
		return findIdenticalF(key, (const AVLNode*)&this->nil, _cmp);
	}
	template<class K1>
	const_iterator findRightT(const K1 &key)const{
		return findLeastRightF(key, (const AVLNode*)&this->nil, _cmp);
	}
	template<class K1>
	const_iterator findLeftT(const K1 &key)const{
		return findMostLeftF(key, (const AVLNode*)&this->nil, _cmp);
	}
	const_iterator findRight(const K &key)const{
		return findLeastRightF(key, (const AVLNode*)&this->nil, _cmp);
	}
	const_iterator findLeft(const K &key)const{
		return findMostLeftF(key, (const AVLNode*)&this->nil, _cmp);
	}
	const_iterator find(const K &key) const{
	//	return _find(key);
		return findIdenticalF(key, (const AVLNode*)&this->nil, _cmp);
	}
	const_iterator insert(const K& key, const V &val, bool replace=true){
		return insertT(key, val, replace);
	}
	bool remove(const K &key){return removeT(key);}
	V& operator[](const K &key){
		return insert(key, V(), false)._n->val;
	}
	const_iterator begin()const{return this->nil.next();}
	const_iterator end()const{return &this->nil;}
};

#endif // AVLTREE_H
