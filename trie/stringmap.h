#ifndef STRINGMAP_H
#define STRINGMAP_H

#include "alloc.h"
#include <string.h>
#include <stdio.h>
//data structure for long string keys,
//which can have long common prefix
//
//based on AVL tree.

class StringTreeBase{
protected:
	struct _Node;
	void _balance(_Node *x);
	int _sz;
	typedef unsigned int uint;
	struct _Node{
		char *key;
		uint h, sl, sr;
		_Node *l;
		_Node *r;
		_Node *p;
		_Node *next;
		_Node *prev;
		_Node(){key = (char*)"";}
		_Node(const char *k){
			int l = (int)strlen(k);
			if(!l)key = (char*)"";
			else{
				key = (char*)malloc(l+1);
				for(int i=0; i<=l; i++)
					key[i]=k[i];
			}
		}
		_Node(const _Node& n){
			int l = (int)strlen(n.key);
			if(!l)key = (char*)"";
			else{
				key = (char*)malloc(l+1);
				for(int i=0; i<=l; i++)
					key[i]=n.key[i];
			}
		}
		~_Node(){if(key&&*key)free(key);}
	};
	_Node *_nil;
	_Node *_find(const char *str)const;//for insert
	_Node *_find0(const char *str);
	void _insert(_Node *before, _Node *n);
	void _delete(_Node *n);
	//_TreeNode *_findRight(const char *str, int len=-1);
public:
	int size()const{return _sz;}
	StringTreeBase(_Node *nil);
	~StringTreeBase();
};

template<class T> class StringMap;
template<class T>
class StringTree : public StringTreeBase{
	typedef StringTreeBase _base;
	friend class StringMap<T>;
	struct _TNode : public _base::_Node{
		T val;
		_TNode(){}
		template<class Y>
		_TNode(const Y&v):val(v){}
		_TNode(const char *k):_base::_Node(k){}
		template<class Y>
		_TNode(const Y&v, const char *k):_base::_Node(k),val(v){}
		~_TNode(){}
	};
	struct _Nilnode:public _TNode{
		mutable int refs;
		const char *k1;
		StringTree<T> *tree;
	};
	AllocRef<_TNode> _al;
	_Nilnode __nil; //contains default value;
	//===============
	_TNode *_copy(_Node *n, _Node *& t){
		if(!n->h)return &__nil;
		_TNode *nn = _al.get_new(*(_TNode*)n);
		nn->l = _copy(n->l);
		nn->prev = t; t->next = nn;
		t = nn;
		nn->r = _copy(n->r, t);
		return nn;
	}
	bool _set_alloc(AllocRef<_TNode> &al){
		if(this->_sz)return false;
		_al = al;
		return true;
	}
public:
//	bool _setAlloc
	class const_iterator{
	protected:
		mutable _TNode *ptr;
	public:
		const_iterator(const _base::_Node *n){ptr = (_TNode*)n;}
		const_iterator(){ptr=0;}
		const_iterator& operator++(){ptr = (_TNode*)ptr->next; return *this;}
		const_iterator& operator--(){ptr = (_TNode*)ptr->prev; return *this;}
		const_iterator next(){return ptr->next;}
		const_iterator prev(){return ptr->prev;}
		const_iterator operator++(int){_TNode *v = ptr; ptr = (_TNode*)ptr->next; return v;}
		const_iterator operator--(int){_TNode *v = ptr; ptr = (_TNode*)ptr->prev; return v;}
		const T* operator->()const{return &ptr->val;}
		const T& operator*()const{return ptr->val;}
		const StringTree<T>& tree()const{                   /*              ptr          ptr__      */
			if(!ptr->h)return ((_Nilnode*)ptr)->tree;       /* ptr=nil or   / \    or         \     */
			if(!ptr->r->h)return ((_Nilnode*)ptr->r)->tree; /*                nil             /\    */
			return ((_Nilnode*)ptr->next->l)->tree;         /*                            next  \   */
		}                                                   /*                           nil \___\  */

		bool operator == (const_iterator it)const{return ptr==it.ptr;}
		bool operator != (const_iterator it)const{return ptr!=it.ptr;}
		bool operator < (const_iterator it)const{return strcmp(ptr->key, it.ptr->key)<0;}
		bool operator > (const_iterator it)const{return strcmp(ptr->key, it.ptr->key)>0;}
		bool operator <= (const_iterator it)const{return strcmp(ptr->key, it.ptr->key)<=0;}
		bool operator >= (const_iterator it)const{return strcmp(ptr->key, it.ptr->key)>=0;}
		const char *key()const{return ptr->key;}
		bool isValid()const{return ptr && ptr->h;}
		bool operator!()const{return !ptr || !ptr->h;}
	};
	class iterator : public const_iterator{
	public:
		iterator(_base::_Node *n):const_iterator(n){}
		iterator(){}
		iterator& operator++(){this->ptr = (_TNode*)this->ptr->next; return *this;}
		iterator& operator--(){this->ptr = (_TNode*)this->ptr->prev; return *this;}
		iterator next(){return this->ptr->next;}
		iterator prev(){return this->ptr->prev;}
		iterator operator++(int){_TNode *v = this->ptr; this->ptr = (_TNode*)this->ptr->next; return v;}
		iterator operator--(int){_TNode *v = this->ptr; this->ptr = (_TNode*)this->ptr->prev; return v;}
		T* operator->(){return &this->ptr->val;}
		T& operator*(){return this->ptr->val;}
		StringTree<T>& tree(){return (StringTree<T>&)(const_iterator::tree());}
	};
/*	bool check(_Node *n){
		if(!n->h)return true;
		bool r=true;
		if(abs(n->l->h - n->r->h)>1){printf("error : node %s disbalanced\n", n->key); r=false;}
		if(n->l->h){
			if(n->sl!=std::min(n->l->sl, n->l->sr)){
				printf("error in node %s : n->sl != Min(n->l->sl, n->l->sr)\n", n->key), r=false;
			}
			if(n->l->p!=n)printf("error in node %s : x->l->p != x\n", n->key), r=false;
			r=r&&check(n->l);
		}
		if(n->r->h){
			if(n->sr!=std::min(n->r->sl, n->r->sr)){
				printf("error in node %s : n->sr != Min(n->r->sl, n->r->sr)\n", n->key), r=false;
			}
			if(n->r->p!=n)printf("error in node %s : x->l->p != x\n", n->key), r=false;
			r=r&&check(n->r);
		}
		return r;
	}
	bool check(){return check(_nil->l);}
	void print(_Node *n){
		if(!n->h)return;
		print(n->l);
		for(int i = n->h; i<=10; i++)printf("  ");
		printf("%2d [%d %d] (%s %d)\n", n->h, n->sl, n->sr, n->key,((_TNode*)n)->val);
		print(n->r);
	}
	void print(){print(__nil.l);}*/
	struct InsRet{
		iterator i;
		bool nodeCreated;
		InsRet(){nodeCreated = false;}
		InsRet(_TNode *n, bool b):i(n){nodeCreated = b;}
	};

	StringTree():StringTreeBase(&__nil){__nil.refs = 1;}
	StringTree(const StringTree<T> &t):StringTreeBase(&__nil){
		__nil.tree = this;
		__nil.l = _copy(t.__nil.l, __nil.prev);
	}
	StringTree<T> & operator=(const StringTree<T> &t){
		clear();
		__nil.l = _copy(t.__nil.l, __nil.prev);
		return *this;
	}
	StringTree<T>& clear(){
		if(__nil.next!=&__nil)
			for(_Node *n = __nil.next->next; ; n = n->next){
				_al.free((_TNode*)n->prev);
				if(n == &__nil)break;
			}
		__nil.next = __nil.prev = __nil.l = __nil.p = &__nil;
		return *this;
	}
	~StringTree(){
		clear();
	}
	template<class Y>
	InsRet insert(const char *str, const Y& v, bool replace=true){
		_TNode * n = (_TNode*)_find0(str), * nn;
		if(n->h){
			if(replace)n->val = v;
			return InsRet(n, false);
		}
		this->_insert(n->r, nn = _al.get_new(v, str));
		return InsRet(nn, true);
	}
	iterator erase(const_iterator it){
		if(it.isValid()){
			iterator r = it.next();
			this->_delete(it.ptr);
			_al.free(it.ptr);
			return r;
		}
		return it;
	}
	bool remove(const char *key){
		_TNode * n = (_TNode*)_find(key);
		if(!n->h)return false;
		_delete(n);
		_al.free(n);
		return true;
	}
	struct RetVal{
		_TNode *n;
		operator const T&(){return n->val;}
		template<class Y>
		T& operator = (const Y&y){
			if(n->h)return n->val = y;
			return *((_Nilnode *)n)->tree->insert(n->k1, y).i;
		}
		RetVal(const RetVal& rv){n=rv.n;}
		RetVal& operator=(const RetVal &rv){n=rv.n; return *this;}
		friend class StringTree<T>;
	private:
		RetVal(_TNode *nn){n=nn;}
	};
	const T& operator[](const char *key)const{
		return ((_TNode*)this->_find(key))->val;
	}
	T& operator [](const char *key){//???????
		_TNode *r = ((_TNode*)this->_find0(key)), *nn;
		if(r->h)return r->val;
		_insert(r->r, nn = _al.get_new(key));
		return nn->val;
	}
	const T& value(const char *key)const{return ((_TNode*)_find(key))->val;}

	iterator find(const char *key){return _find(key);}
	const_iterator find(const char *key)const{return _find(key);}

	iterator begin(){return this->_nil->next;}
	const_iterator begin()const{return this->_nil->next;}
	const_iterator constBegin()const{return this->_nil->next;}
	iterator end(){return this->_nil;}
	const_iterator end()const{return this->_nil;}
	const_iterator constEnd()const{return this->_nil;}
};

template<class T>
class StringMap{
	int _sz, _hsz;
	int _hash(const char *str, unsigned int mask)const{
		//unsigned int res=str[0];
		int l = strlen(str);
		return mask&(unsigned int)(str[0] ^ (str[l-1]<<5) ^ (str[641356789&(l-1)]<<10));
	/*	for(; str[0] && str[1] && str[2] && str[3]; str+=4){
			res = (res * *(int*)str) + str[2];
		}
		return res & mask;*/
	}
	AllocRef<typename StringTree<T>::_TNode> _al;
	StringTree<T> *_t;
	void expand(){
		if(!_hsz){
			_hsz = 8;
			_t = new StringTree<T>[8];
			for(int i=0; i<_hsz; i++)
				_t[i]._set_alloc(_al);
			return;
		}
		int nh = 2*_hsz, nm = nh-1;
		StringTree<T> *t = new StringTree<T>[nh];
		for(int i=0; i<nh; i++)
			t[i]._set_alloc(_al);
		for(int i=0; i<_hsz; i++){
			for(typename StringTree<T>::iterator it = _t[i].begin(); it.isValid(); ++it){
				t[_hash(it.key(), nm)].insert(it.key(), *it);
			}
		}
		_hsz = nh;
		delete []_t; _t = t;
	}
protected:

/*	struct _Node{
		const char *key;
		union{
			void *value;
			StringTreeBase *tree;
		};
	};*/

public:
	int size()const{return _sz;}
	StringMap(){_sz=_hsz=0; _t=0;}
	template<class Y>
	bool insert(const char *k, const Y& v, bool replace = true){
		if(_hsz<_sz+1)expand();
		if(_t[_hash(k, _hsz-1)].insert(k, v, replace).nodeCreated){
			_sz++; return true;
		}
		return false;
	}
	bool remove(const char *k){
		if(!_sz)return false;
		if(_t[_hash(k, _hsz-1)].remove(k)){
			_sz--; return true;
		}
		return false;
	}
	const T &value(const char *k, const T &def)const{
		if(!_sz)return def;
		typename StringTree<T>::const_iterator it = _t[_hash(k, _hsz-1)].find(k);
		if(it.isValid())return *it;
		return def;
	}
	T &value(const char *k, T &def){
		typename StringTree<T>::iterator it = _t[_hash(k, _hsz-1)].find(k);
		if(it.isValid())return *it;
		return def;
	}
};

#endif // STRINGMAP_H
