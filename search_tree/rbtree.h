#include "alloc.h"
#include "commontree.h"
//#define USE_NATIVE_POINTERS

class RBTBase{//basic PST class
protected:
	enum color{BLACK=0, RED=1};
	struct RBTNodeBase{
		/*		int key, kr;
		int rmost;*/
#if defined(_WIN64) && !defined(USE_NATIVE_POINTERS)
		unsigned long long L, H;
		RBTNodeBase *setL(RBTNodeBase *ll){
			L = (L & 0xFFFFFF0000000000ull) | ((char*)ll - (char*)0);
			return ll;
		}
		RBTNodeBase *setR(RBTNodeBase *ll){
			L = (L & 0xFFFFFFFFFFull) | (((char*)ll - (char*)0) << 40);
			H = (H & 0xFFFFFFFFFFFF0000ull) | (((char*)ll - (char*)0) >> 24);
			return ll;
		}
		RBTNodeBase *setP(RBTNodeBase *ll){
			H = (H & 0xFF0000000000FFFFull) | (((char*)ll - (char*)0) << 16);
			return ll;
		}
		RBTNodeBase *left(){ return (RBTNodeBase*)((char*)0 + (L & 0xFFFFFFFFFFull)); }
		RBTNodeBase *right(){ return (RBTNodeBase*)((char*)0 + ((L >> 40) | ((H & 0xFFFFull) << 24))); }
		RBTNodeBase *up(){ return (RBTNodeBase*)((char*)0 + ((H >> 16) & 0xFFFFFFFFFFull)); }
		color c(){ return color((H >> 62) & 1); }
		void setC(color c){ H = (H & 0xBFFFFFFFFFFFFFFFull)|((unsigned long long)(c)<<62); }
		void setNil(){ H |= 1ull << 63; }
		bool isNil()const{ return (H>>63)!=0; }

		RBTNodeBase(){ H &= 0x7FFFFFFFFFFFFFFFull; }
		RBTNodeBase *next(){ return nextNodeF(this); }
		RBTNodeBase *prev(){ return prevNodeF(this); }
#else
		RBTNodeBase *l;
		RBTNodeBase *r;
		RBTNodeBase *p;
		color _c:2;
		unsigned int is_nil:1;
		RBTNodeBase *setL(RBTNodeBase *ll){ return l = ll; }
		RBTNodeBase *setR(RBTNodeBase *ll){ return r = ll; }
		RBTNodeBase *setP(RBTNodeBase *ll){ return p = ll; }
		RBTNodeBase *left(){ return l; }
		RBTNodeBase *right(){ return r; }
		RBTNodeBase *up(){ return p; }
		RBTNodeBase *next(){ return nextNode(this); }
		RBTNodeBase *prev(){ return prevNode(this); }
        const RBTNodeBase *next()const{ return nextNode(this); }
        const RBTNodeBase *prev()const{ return prevNode(this); }

        color c()const{return _c;}
		void setC(color c){_c=c;}
		bool isNil()const{ return is_nil; }
		void setNil(){ is_nil = 1; }
		RBTNodeBase(){ is_nil = 0; }
#endif
	};
/*	struct RBTNodeBase{
		RBTNodeBase(){is_nil = false;}
		RBTNodeBase *next(){return nextNode(this);}
		RBTNodeBase *prev(){return prevNode(this);}
		RBTNodeBase *l, *r, *p;
		color c:2;
		unsigned int is_nil:1;
		bool isNil()const{return is_nil;}
	};*/
	//struct RBTNil:public RBTNodeBase{}
	RBTNodeBase * rightrot(RBTNodeBase *n);
	RBTNodeBase * leftrot(RBTNodeBase * n);
	RBTNodeBase *rightlongrot(RBTNodeBase * n);
	RBTNodeBase * leftlongrot(RBTNodeBase *n);
	void insbalance(RBTNodeBase *x);
	void delbalance(RBTNodeBase *x, RBTNodeBase *p);
	bool _remove(RBTNodeBase *t);
//	bool _enumerate(RBTNodeBase *n, int l, int r, int b, void *obj, bool push(void*, RBTNodeBase*));
//	RBTNodeBase *_minY(int l, int r, int b, RBTNodeBase *n);
//	bool _contains(const Point & pt)const{return !_find(pt)->is_nil;}
//	const RBTNodeBase* _find(const Point & pt)const;
//	bool extractMaxQX(RBTNodeBase*& t, RBTNodeBase *&ep, bool &duplAsP);
//	void disposeP(RBTNodeBase *t);
//	void extractP(RBTNodeBase *t);
//	virtual RBTNodeBase *_insnode(int x, int y)=0;
	RBTNodeBase *_first;
	RBTNodeBase nil;
	int sz;
	void print(RBTNodeBase *n, int rec);
	int check(RBTNodeBase *n);
	//int checkPST(PSTNodeBase *n);
public:
	bool check(){return check(nil.left())>=0 && nil.left()->c()==BLACK &&
		(nil.left()==&nil||nil.left()->up() == &nil);}
	void print(){print(nil.left(), 2);}
//	bool contains(int x, int y)const;
	int size()const{return sz;}

	RBTBase();
	~RBTBase();

	template<class T>
	struct _default_cmp{
		bool less(const T&x, const T&y)const{return x<y;}
		bool equal(const T&x, const T&y)const{return !(x<y || y<x);}
		int compare(const T& x, const T&y)const{
			return x<y ? -1 : y<x ? 1 : 0;
		}
	};
};

template<class K, class T, class Cmp = RBTBase::_default_cmp<K> >
class RBT : public RBTBase{
	typedef RBTBase::RBTNodeBase _nodebase;
	struct RBTNode : public RBTBase::RBTNodeBase{
		K key;
		T val;
		RBTNode()= default;
		RBTNode(const K&k, const T&v):key(k),val(v){}
	};
	typedef AllocRef<RBTNode> _al_tp;
	_al_tp _al;
	Cmp _cmp;
	void _delrec(_nodebase *n){
		for(_nodebase *nn = n;!nn->isNil(); nn = n){
			n = nn->left();
			_delrec(nn->right());
			_al.free((RBTNode*)nn);
		}
	}
public:
	class const_iterator{
		RBTNode *_n;
		explicit const_iterator(const RBTNodeBase *n){_n=(RBTNode*)n;}
		friend class RBT<K,T,Cmp>;
	public:
		const_iterator(){_n=0;}
		bool isValid()const{return _n && !_n->isNil();}
		const T& value()const{return _n->val;}
		const K& key()const{return _n->key;}
		const T* operator->()const{return &_n->val;}
		const_iterator& operator++(){
			_n = (RBTNode*)_n->next();
			return *this;
		}
		const_iterator& operator--(){
			_n = (RBTNode*)_n->prev();
			return *this;
		}
		const_iterator operator++(int){
			RBTNode* old = _n;
			_n = (RBTNode*)_n->next();
			return old;
		}
		const_iterator operator--(int){
			RBTNode* old = _n;
			_n = (RBTNode*)_n->prev();
			return old;
		}
		bool operator == (const_iterator it)const{return _n==it._n;}
		bool operator != (const_iterator it)const{return _n!=it._n;}
	};
	RBT():RBTBase(){}
	~RBT(){clear();}

	bool setAlloc(const _al_tp &al){
		if(!this->nil.l->is_nil)return false;
		_al = al;
		return true;
	}
	void clear(){
		_delrec(this->nil.left());
		this->nil.setL(&this->nil);
		this->sz = 0;
	}
	size_t memory_usage()const{
		return _al.memory_usage();
	}
	template<class K1, class V1>
	const_iterator insertT(const K1& key, const V1 &val, bool replace=true){
		insertPlaceResult<RBTNode> pn = findInsertPlaceF(key, (RBTNode*)&this->nil, _cmp);
		if(!pn.n->isNil() && !pn.c){
			if(replace)((RBTNode*)pn.n)->val = val;
			return const_iterator(pn.n);
		}
		RBTNode *newnode = _al.get_new(key, val);
		newnode->setP(pn.n);
		newnode->setL(&this->nil);
		newnode->setR(&this->nil);
		newnode->setC(RED);
		if(pn.c>=0){
			pn.n->setL(newnode);
		}else{
			pn.n->setR(newnode);
		}
		this->insbalance(newnode);
		this->sz++;
		return const_iterator(newnode);
	}
	const_iterator erase(const_iterator it){
		if(it.isValid()) {
			_nodebase *n = it._n->next();
			this->_remove(it._n);
			_al.free(it._n);
			this->sz--;
			return const_iterator(n);
		}
        return it;
	}
	template<class K1>
	const_iterator findT(const K1 &key)const{
		return const_iterator(findIdenticalF(key, (const RBTNode*)&this->nil, _cmp));
	}
	template<class K1>
	const_iterator findRightT(const K1 &key)const{
		return const_iterator(findLeastRightF(key, (const RBTNode*)&this->nil, _cmp));
	}
	template<class K1>
	const_iterator findLeftT(const K1 &key)const{
		return const_iterator(findMostLeftF(key, (const RBTNode*)&this->nil, _cmp));
	}
	const_iterator findRight(const K &key)const{
		return const_iterator(findLeastRightF(key, (const RBTNode*)&this->nil, _cmp));
	}
	const_iterator findLeft(const K &key)const{
		return const_iterator(findMostLeftF(key, (const RBTNode*)&this->nil, _cmp));
	}
	const_iterator find(const K &key) const{
		return const_iterator(findIdenticalF(key, (const RBTNode*)&this->nil, _cmp));
	}
	template<class K1>
	bool removeT(const K1& key){
		const_iterator n(findT(key));
		return n != erase(n);
	}
	bool remove(const K& key){return removeT(key);}
	const_iterator insert(const K& key, const T& val, bool replace = true){
		return insertT(key, val, replace);
	}
	const_iterator find(const K &key){
		return const_iterator(findIdenticalF(key, (const RBTNode*)&this->nil, _cmp));
	}
	T& operator[](const K &key){
		return insert(key, T(), false)._n->val;
	}
	const_iterator begin()const{return const_iterator(this->nil.next());}
	const_iterator end()const{return const_iterator(&this->nil);}
};
