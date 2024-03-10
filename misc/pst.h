#include "alloc.h"

#ifdef COUNT_PST_OPERATIONS
#include <stdio.h>
struct IntCounter{
	int n;
	const char *nm;
	IntCounter(const char * name){n=0;nm = name;}
	~IntCounter(){printf("%s counter = %d\n",nm,n);}
};
#define SETUP_COUNTER static IntCounter ic(__FUNCTION__);\
	ic.n++;
#else
#define SETUP_COUNTER
#endif
class PSTBase{//basic PST class
protected:
	enum color{BLACK=0, RED=1};
public:
	struct PSTNodeBase;
#ifdef _WIN64
	typedef Pointer32<PSTNodeBase> ptr_t;
#else
	typedef PSTNodeBase* ptr_t;
#endif
	//typedef PSTNodeBase* ptr_t;
	struct Point{
		int x, y;
		Point(){}
		Point(int X, int Y){x=X;y=Y;}
		bool inRange(int l, int r, int b)const{
			SETUP_COUNTER
			return x<=r && x>=l && y<=b;
		}
		bool operator == (Point p2)const{
			SETUP_COUNTER
			return x==p2.x && y==p2.y;
		}
		bool operator < (Point p2)const{
			SETUP_COUNTER
			return x<p2.x || (x == p2.x && y<p2.y);
		}
		bool operator != (Point p2)const{
			SETUP_COUNTER
			return x!=p2.x || y!=p2.y;
		}
		int cmp(Point p2)const{
			SETUP_COUNTER
			return x<p2.x ? -1 :x>p2.x ? 1 :
				y<p2.y ? -1 : y>p2.y ? 1 : 0;
		}
	};
public:
#ifdef _WIN64
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(8)     /* set alignment to 8 byte boundary */
#endif
	struct PSTNodeBase{
		PSTNodeBase(){p=0; duplQ = false; is_nil = false;}
		//PSTNodeBase *p, *l, *r;
		Point q;
		ptr_t p, l, r;
		struct{
			unsigned int duplQ:1;
			unsigned int is_nil:1;
			color c:2;
		};
	};
#ifdef _WIN64
#pragma pack(pop)
#endif

protected:
	void rightrot(ptr_t& n);
	void leftrot(ptr_t& n);
	void rightlongrot(ptr_t& n);
	void leftlongrot(ptr_t& n);
	bool insbalance(ptr_t&p, PSTNodeBase *l);
	bool delbalance(ptr_t&p, PSTNodeBase *l);
	PSTNodeBase * __insert(Point pt);
	bool _remove(ptr_t &t, const Point &pt);
	PSTNodeBase * _remove(int x, int y){
		_del = &nil;
		_remove(nil.l, Point(x,y));
		nil.l->c = BLACK;
		return _del;
	}
	PSTNodeBase * _insert(Point p){
		PSTNodeBase * res = __insert(p);
		nil.l->c = BLACK;
		return res;
		//_insert(nil.l, p, true);
	}
	bool _enumerate(PSTNodeBase *n, int l, int r, int b, void *obj, bool push(void*, PSTNodeBase*));
	PSTNodeBase *_minY(int l, int r, int b, PSTNodeBase *n);
	bool _contains(const Point & pt)const{return !_find(pt)->is_nil;}
	const PSTNodeBase* _find(const Point & pt)const;
	bool extractMaxQX(ptr_t& t, PSTNodeBase *&ep, bool &duplAsP);
	void disposeP(PSTNodeBase *t);
	void extractP(PSTNodeBase *t);
	virtual PSTNodeBase *_insnode(int x, int y)=0;
	PSTNodeBase *_del;
	PSTNodeBase nil;
	int sz;
	void print(PSTNodeBase *n, int rec, bool sh);
	int check(PSTNodeBase *n);
	//int checkPST(PSTNodeBase *n);
public:
	bool check(){return check(nil.l)>=0 && nil.l->c==BLACK;}
	void print(bool sh){print(nil.l, 2, sh);}
	bool contains(int x, int y)const;
	int size()const{return sz;}

	PSTBase();
	~PSTBase();
};

template<class T>
class PST : public PSTBase{
	typedef PSTBase::PSTNodeBase _nodebase;
public:
	struct PSTNode : public PSTBase::PSTNodeBase{
		T val;
		PSTNode(){}
		PSTNode(int x, int y, const T&v):val(v){this->q.x = x; this->q.y = y;}
	};
private:
	typedef AllocRef<PSTNode> _al_tp;
	_al_tp _al;
/*	template<class List, class R, class V, R List::*push(V)>
	bool _pushVal(void *obj, _nodebase *n){((List*)obj)->*push(((PSTNode*)n)->val); return true;}
	template<class List, class R, R List::*push(int, int)>
	bool _pushXY(void *obj, _nodebase *n){((List*)obj)->*push(n->q.x, n->q.y); return true;}
	template<class List, class V, bool List::*push(V)>
	bool _pushCheckVal(void *obj, _nodebase *n){return ((List*)obj)->*push(((PSTNode*)n)->val);}
	template<class List, bool List::*push(int, int)>
	bool _pushCheckXY(void *obj, _nodebase *n){return ((List*)obj)->*push(n->q.x, n->q.y);}*/
	template<class R, class V>
	struct _find_objV{
		R (*push)(V);
		R (*pushxy)(int,int);
		static bool pushV(void *obj, _nodebase *n){
			(((_find_objV*)obj)->push)(((PSTNode*)n)->val);
			return true;
		}
		static bool pushXY(void *obj, _nodebase *n){
			(((_find_objV*)obj)->push)(((PSTNode*)n)->val);
			return true;
		}
	};
	template<class L, class R, class V>
	struct _find_objL{
		L *obj;
		R (L::*push)(V);
		R (L::*pushxy)(int,int);
		static bool pushV(void *o, _nodebase *n){
			R (L::*p)(V);
			p = ((_find_objL<L,R,V>*)o)->push;
			L &obj = *((_find_objL<L,R,V>*)o)->obj;
			(obj.*p)(((PSTNode*)n)->val);
			return true;
		}
		static bool pushXY(void *o, _nodebase *n){
			R (L::*p)(int,int);
			p = ((_find_objL<L,R,V>*)o)->pushxy;
			L &obj = *((_find_objL<L,R,V>*)o)->obj;
			(obj.*p)(n->q.x,n->q.y);
			return true;
		}
	};
	void _delrec(_nodebase *n){
		for(_nodebase *nn = n;!nn->is_nil; nn = n){
			n = nn->l;
			_delrec(nn->r);
			_al.free((PSTNode*)nn);
		}
	}
	const T* _ptrv; //for insert
	_nodebase* _insnode(int x, int y){
		return _al.get_new(x, y, *_ptrv);
	}
public:
	class ConstPointer{
		PSTNode *_n;
		ConstPointer(PSTNodeBase *n){_n=(PSTNode*)n;}
		friend class PST<T>;
	public:
		ConstPointer(){_n=0;}
		bool isValid()const{return _n && !_n->is_nil;}
		const T& value()const{return _n->val;}
		const T* operator->()const{return &_n->val;}
		int x()const{return _n->q.x;}
		int y()const{return _n->q.y;}
	};
	PST():PSTBase(){}
	~PST(){clear();}

	ConstPointer minY(int l, int r, int b){
		return this->_minY(l,r,b,this->nil.l);
	}
	bool setAlloc(const _al_tp &al){
		if(!this->nil.l->is_nil)return false;
		_al = al;
		return true;
	}
	void clear(){
		_delrec(this->nil.l);
		this->nil.l = &this->nil;
		this->sz = 0;
	}
	template<class R>
	void findPairs(int l, int r, int b, R push(int,int)){
		_find_objV<R,void> o; o.pushxy = push;
		return _enumerate(this->nil.l, l, r, b, &o, &_find_objV<R,void>::pushXY);
	}
	template<class L, class R>
	void findPairs(int l, int r, int b, L& ls, R L::*push(int,int)){
		_find_objL<L,R,void> o; o.pushxy = push; o.obj = &ls;
		return _enumerate(this->nil.l, l, r, b, &o, &_find_objV<R,void>::pushXY);
	}
	template<class R, class V>
	void findValues(int l, int r, int b, R push(V)){
		_find_objV<R,V> o; o.push = push;
		return _enumerate(this->nil.l, l, r, b, &o, &_find_objV<R,V>::pushV);
	}
	template<class L, class R, class V>
	void findValues(int l, int r, int b, L& ls, R (L::*push)(V)){
		_find_objL<L,R,V> o; o.push = push; o.obj = &ls;
		_enumerate(this->nil.l, l, r, b, &o, &_find_objL<L,R,V>::pushV);
	}
	bool insert(int x, int y, const T &v, bool replace=true){
		_ptrv = &v;
		int psz = this->sz;
		PSTNode *n = (PSTNode*)this->_insert(Point(x,y));//this->_find(Point(x,y));
		if(psz==sz){
			if(replace)n->val = v;
			return false;
		}
		return true;
	}
	bool remove(int x, int y){
		PSTNode *n = (PSTNode*)this->_remove(x,y);
		if(n->is_nil)return false;
		--this->sz;
		_al.free(n);
		return true;
	}
};
