#pragma once
//#define USE_ALLOC_DEL_NEW
#include <cstdlib>
#include <new>
#include <memory>
//#include <stddef.h>
//#include "../common/commonfunctions.h"

template<class T>
void _destruct(T* x) {
    if constexpr (std::is_destructible_v<T>)
        x->~T();
}

template<class T, class Y>
T * _construct(T *dst, const Y &val, size_t sz){
	if(!sz)return dst;
	if(!dst)dst = (T*)malloc(sz*sizeof(T));
	if(!dst)return 0;
	for(size_t i=0; i<sz; i++)
		::new(dst+i) T(val);
	return dst;
}

template<class T>
T * _construct(T *dst, size_t sz){
	if(!sz)return dst;
	if(!dst)dst = (T*)malloc(sz*sizeof(T));
	if(!dst)return 0;
	for(size_t i=0; i<sz; i++)
		::new(dst+i) T;
	return dst;
}

template<class T, class It>
T * _construct_copy(T *dst, It src, size_t sz){
	if(!sz)return dst;
	if(!dst)dst = (T*)malloc(sz*sizeof(T));
	if(!dst)return 0;
	for(size_t i=0; i<sz; i++, ++src)
		::new(dst+i) T(*src);
	return dst;
}

template<class T> void _destruct(T *arr, size_t sz){
	for(size_t i=0; i<sz; i++)
		_destruct(arr+i);
}

template<class T> class AllocRef;

template<class T>
void inc_used(T *p){
    if(p)p->used++;
}

template<class T>
void dec_used(T *p){
    if(p && !--p->used)delete p;
}
template<class T>
T* Memcpy(T *dest, const T *src, size_t sz){
	for (size_t i = 0; i < sz; i++)
		dest[i] = src[i];
	return dest;
}

template<class T>
class Pointer32{
	uint32_t ptr;
	struct s{
		char c;
		T t;
	};
public:
	Pointer32(){}
	Pointer32(T* p){ptr = (unsigned long)(((char*)p-(char*)0)/offsetof(s,t));}
	T* operator=(T*p){
		ptr = (unsigned long)(((char*)p-(char*)0)/offsetof(s,t));
		return p;
	}
	operator T*()const{return (T*)(ptr*offsetof(s,t));}
	T* operator->()const{return (T*)(ptr*offsetof(s,t));}
};
template<class T>
class Alloc{
	union AllocTp{
		char p[sizeof(T)];
		AllocTp *next;
	};
	struct ListAllocNode{
		size_t sz;
		AllocTp* p;
		ListAllocNode *next;
		ListAllocNode(size_t n, ListAllocNode *nxt=0){
			p = (AllocTp*)(new int[n*sizeof(AllocTp)/sizeof(int)]);
			for(size_t i=1; i<n; i++)
				p[i-1].next = p+i;
			if(n)p[n-1].next = 0;
			next = nxt;
			sz = n;
		}
		~ListAllocNode(){
			::free(p);
		}
	};

	double exp_coef;
	int start_size;
	ListAllocNode *head;
	AllocTp *fr;
	int used;
	size_t sz;
	bool _is_static;
	void _check(){
		if(!fr){
			if(head)head = new ListAllocNode(int(head->sz*exp_coef)+1, head);
			else head = new ListAllocNode(start_size);
			fr = head->p;
		}
	}
	bool _check_node(const T *x)const{
		for(ListAllocNode * p = head; p; p = p->next)
			if(x>=p->p && x - p->p < p->sz)return true;
		return false;
	}

//	friend Alloc<T>* NewAlloc(size_t start_sz, double k);
//	friend void CopyAlloc(Alloc<T> *&to, Alloc<T> *from);
//	friend void DeleteAlloc(Alloc<T> *p);

	//friend class AllocRef<T>;
	void destroy(){
		ListAllocNode *p = head;
		for(;p; p=head){
			head = p->next;
			delete p;
		}
		head = 0;
	}
	void destroy_other(){
		if(!head)return;
		ListAllocNode *p;
		for(p = head; p->next; p = head){
			head = p->next;
			delete p;
		}
		head->next = 0;
		fr = head->p;
		for(size_t i=1; i<head->sz; i++)
			head->p[i-1].next = head->p+i;
		head->p[head->sz-1].next = 0;
	}
    template<class Y>
    friend void inc_used(Y *p);
    template<class Y>
    friend void dec_used(Y *p);
public:
	//void inc_used(){if(this)used++;}
	//void dec_used(){if(this)used--;}
	int is_used()const{return used;}
	size_t count()const{ return sz; }
	bool is_static()const{return _is_static;}
	explicit Alloc(size_t start_sz=10, double k=1.1){
#ifdef _DEBUG
		if(k<1)throw std::std::runtime_error("Alloc : k<1");
#endif
		start_size = (int)start_sz;
		exp_coef = k;
		head = 0; sz = 0;
		used = 1;
		_is_static = true;
		fr = 0;
	}
	T* get_new(){
		_check();
		T *p = (T*)fr;
		fr = fr->next; sz++;
		::new(p) T();
		return p;
	}
	template<class Y>
	T* get_new(const Y &y){
		_check();
		T *p = (T*)fr;
		fr = fr->next; sz++;
		::new(p) T(y);
		return p;
	}
	template<class Y, class Z>
	T* get_new(const Y &y, const Z &z){
		_check();
		T *p = (T*)fr;
		fr = fr->next; sz++;
		::new(p) T(y, z);
		return p;
	}
	template<class Y, class Z, class W>
	T* get_new(const Y &y, const Z &z, const W &w){
		_check();
		T *p = (T*)fr;
		fr = fr->next; sz++;
		::new(p) T(y, z, w);
		return p;
	}
	template<class Y, class Z, class U, class W>
	T* get_new(const Y &y, const Z &z, const U& u, const W &w){
		_check();
		T *p = (T*)fr;
		fr = fr->next; sz++;
		::new(p) T(y, z, u, w);
		return p;
	}
	template<class Y, class Z, class U, class V, class W>
	T* get_new(const Y &y, const Z &z, const U& u, const V&v, const W &w){
		_check();
		T *p = (T*)fr;
		fr = fr->next; sz++;
		::new(p) T(y, z, u, v, w);
		return p;
	}
	void free(T *n){
#ifdef _DEBUG_ALLOCATION
		if(!_check_node(n))throw "Alloc::free : node isn't from current allocator";
#endif
		_destruct(n);
		((AllocTp*)n)->next = fr;
		fr = ((AllocTp*)n);
		if(!--sz)destroy_other();
	}
	~Alloc(){
		destroy();
	}
};
using index_t = uint32_t; // it must be a 32-bit unsigned integer
class IndexedAlloc{
	unsigned int obj_size, block_mask, block_shift;
	char **p; // valid index range = [1 ... used_blocks]
	unsigned int max_blocks, used_blocks;
	index_t fr, sz;

	mutable int used;
	bool _is_static;
	bool _check(){
		if (!fr){
			index_t s, i, p0 = (used_blocks+1) << block_shift;
			if (used_blocks == max_blocks){
				char ** p1 = 0;
				s = 3 * used_blocks / 2 + 2;
				try{
					p1 = new char*[s];
				}
				catch (...){}
				if (!p1)return false;
				p1--;
				max_blocks = s;
				for (i = 1; i <= used_blocks; i++)
					p1[i] = p[i];
				delete[] (p+1);
				p = p1;
			}
			s = size_t(obj_size) << block_shift;
			char *pp = 0;
			try{
				pp = p[used_blocks+1] = new char[sz];
			}
			catch (...){}
			if (!pp)return false;
			used_blocks++;
			for (i = 1; i < s; i += sz){
				*(index_t*)(pp + i - 1) = p0 + i;
			}
			*(index_t*)(pp + s - 1) = 0;
			fr = p0;
			return true;
		}
        return true;
	}
	bool _check_node(index_t x)const{
		return x > block_mask && x < (used_blocks << block_shift);
	}

	//	friend Alloc<T>* NewAlloc(size_t start_sz, double k);
	//	friend void CopyAlloc(Alloc<T> *&to, Alloc<T> *from);
	//	friend void DeleteAlloc(Alloc<T> *p);

	//friend class AllocRef<T>;
	void destroy(int r = 0){
		unsigned int i;
		for (i = r + 1; i <= used_blocks; i++)
			delete[]p[i];
		if (!r){
			delete[]p;
			p = 0;
			max_blocks = 0;
		}
		used_blocks = r;
		sz = 0;
	}
    template<class T>
    friend void inc_used(T *p);
    template<class T>
    friend void dec_used(T *p);
	int is_used(){ return used; }
	friend class IndexedAllocRef;
public:
	bool is_static(){ return _is_static; }
	IndexedAlloc(unsigned int data_size = 0, index_t block_sh = 6){
		used_blocks = max_blocks = 0;
		p = 0;
		used = 1;
		_is_static = true;
		sz = fr = 0;
		setSizes(data_size, block_sh);
	}
	void setSizes(unsigned int data_size, index_t block_sh){
#ifdef _DEBUG
		if (sz)throw "IndexedAlloc::setSizes : setting parameters for nonempty structure";
		if (data_size && data_size<sizeof(index_t))throw "IndexedAlloc : data_size < 4";
		if (block_sh >= 30)throw "IndexedAlloc : too big block_shift";
		if ((unsigned long long)(data_size) << block_sh >= (unsigned long long)(1) << 31)throw "IndexedAlloc : too big size of block";
#endif
		destroy();
		block_shift = block_sh;
		obj_size = data_size;
		block_mask = (1<<block_sh)-1;
	}
	void setObjSize(unsigned int data_size){
		setSizes(data_size, block_shift);
	}
	void setBlockSize(unsigned int block_shift){
		setSizes(obj_size, block_shift);
	}
	char *data(index_t i){
#ifdef _DEBUG
		if(!_check_node(i))throw "IndexedAlloc::data : index out of range";
#endif
		return p[i >> block_shift]+(i&block_mask);
	}
	index_t get_new(){
		if (!fr && !_check())return 0;
		sz++;
		index_t r = fr;
		fr = *(index_t*)data(fr);
		return r;
	}
	void free(index_t n){
		if (!n)return;
		char *pn = data(n);
		*(index_t*)pn = fr;
		fr = n;
		if (!--sz)destroy(1);
	}
	~IndexedAlloc(){
		destroy();
	}
};

class ResizableAlloc{
	char *fr;
	size_t obj_size, sz;
	struct ListAllocNode{
		ListAllocNode *next;
		size_t sz;
		char p[1];
		static ListAllocNode *create(size_t os, size_t n, ListAllocNode *nxt = 0){
			ListAllocNode *r = nullptr;
            auto poffset = offsetof(ListAllocNode, p);
            r = (ListAllocNode*)(malloc(n*os + poffset));
			if (!r)return nullptr;
			for (size_t i = os; i < n*os; i += os)
				*(char**)(r->p+i-os) = r->p + i;
			if (n)*(char**)(r->p + (n-1)*os) = nullptr;
			r->next = nxt;
			r->sz = n;
			return r;
		}
	};

	double exp_coef;
	size_t start_size;
	ListAllocNode *head;
	int used;
	bool _is_static;
	bool _check(){
		if (!fr){
			if (head){
				ListAllocNode *h1 = ListAllocNode::create(obj_size, int(head->sz*exp_coef) + 1, head);
				if (!h1)return false;
				head = h1;
			}
			else head = ListAllocNode::create(obj_size, start_size);
			if (!head)return false;
			fr = head->p;
		}
		return true;
	}
	bool _check_node(const char*x)const{
		for (ListAllocNode * p = head; p; p = p->next)
			if (size_t(x - p->p) < obj_size*p->sz)return true;
		return false;
	}

	friend class ResizableAllocRef;
	void destroy(){
		ListAllocNode *p = head;
		for (; p; p = head){
			head = p->next;
			::free(p);
		}
		head = nullptr;
		fr = nullptr;
	}
	void destroy_other(){
		if (!head)return;
		ListAllocNode *p;
		for (p = head; p->next; p = head){
			head = p->next;
			delete p;
		}
		head->next = nullptr;
		fr = head->p;
		for (size_t i = 1; i<head->sz; i++)
			*(char**)(head->p + obj_size*(i - 1)) = head->p + i*obj_size;
		*(char**)(head->p + obj_size*(head->sz - 1)) = nullptr;
	}

    template<class T>
    friend void inc_used(T *p);
    template<class T>
    friend void dec_used(T *p);
	int is_used() const{ return used; }
	friend class IndexedAllocRef;
public:
	bool is_static() const{ return _is_static; }
	explicit ResizableAlloc(size_t data_size = 0, size_t ssz = 10, double exp_koef = 1.1) : obj_size(data_size){
		head = nullptr;
		used = 1;
		_is_static = true;
		sz = 0; fr = nullptr;
		start_size = ssz; exp_coef = exp_koef;
	}
	void setObjSize(size_t data_size){
#ifdef _DEBUG
		if (sz)throw "IndexedAlloc::setSizes : setting parameters for nonempty structure";
		if (data_size && data_size<sizeof(void*))throw "IndexedAlloc : data_size < 4";
#endif
		destroy();
		obj_size = data_size;
	}
	void* get_new(){
		if (!fr && !_check())return nullptr;
		sz++;
		char * r = fr;
		fr = *(char**)(fr);
		return r;
	}
	void free(void* n){
		if (!n)return;
#ifdef _DEBUG_ALLOCATION
		if (!_check_node(n))throw "Alloc::free : node isn't from current allocator";
#endif
		*(char**)n = fr;
		fr = (char*)n;
		if (!--sz)destroy_other();
	}
	~ResizableAlloc(){
		destroy();
	}
};

template<class T> Alloc<T>* NewAlloc(size_t start_sz=10, double k=1.5){
	auto *res = new Alloc<T>(start_sz, k);
//	res->_is_static = false;
	return res;
}

template<class T> void CopyAlloc(Alloc<T> *&to, Alloc<T> *from){
    inc_used(to = from);
}

template<class T> void DeleteAlloc(Alloc<T> *p){
	dec_used(p);
}

class IndexedAllocRef{
	mutable IndexedAlloc *_Al;
public:
	index_t allocated()const{ return _Al->sz; }
	void swap(IndexedAllocRef &ar){
		IndexedAlloc *t = _Al; _Al = ar._Al; ar._Al = t;
	}
	IndexedAllocRef(){ _Al = nullptr; }
	IndexedAllocRef(unsigned int obj_sz, unsigned int block_sz = 6){
		_Al = new IndexedAlloc(obj_sz, block_sz);
		//_Al->inc_used();
	}
	void setSizes(unsigned int obj_sz, unsigned int block_sz){
		if (!_Al)_Al = new IndexedAlloc(obj_sz, block_sz);
		else _Al->setSizes(obj_sz, block_sz);
	}
	void setObjSize(unsigned int obj_sz){
		if (!_Al)_Al = new IndexedAlloc(obj_sz);
		else _Al->setObjSize(obj_sz);
	}
	void setBlockSize(unsigned int block_sz){
		unsigned int bs = 1;
		while ((1u << (bs + 1)) < block_sz)bs++;
		if (!_Al)_Al = new IndexedAlloc(0, bs);
		else _Al->setBlockSize(bs);
	}
	IndexedAllocRef(const IndexedAllocRef &ar){
		inc_used(_Al = ar._Al);
	}
	IndexedAllocRef& operator=(const IndexedAllocRef &ar){
		inc_used(ar._Al);
		dec_used(_Al);
		_Al = ar._Al;
		return *this;
	}
	int refs()const{ return _Al ? _Al->is_used() : 0; }
	index_t get_new(){
		return _Al->get_new();
	}
	void free(index_t n){
		return _Al->free(n);
	}
	~IndexedAllocRef(){
		dec_used(_Al);
		_Al = nullptr;
	}
};

#ifndef USE_ALLOC_DEL_NEW
template<class T>
class AllocRef{
	Alloc<T> *_Al;
public:
	void swap(AllocRef<T> &ar){
		Alloc<T> *t = _Al; _Al = ar._Al; ar._Al = t;
	}
	AllocRef(size_t start_sz=10, double k=1.1){
		_Al = NewAlloc<T>(start_sz, k);
	}
	AllocRef(const AllocRef<T> &ar){
		CopyAlloc(_Al, (Alloc<T>*)ar._Al);
	}
	AllocRef<T>& operator=(const AllocRef<T> &ar){
		if(ar._Al==_Al)return *this;
		DeleteAlloc(_Al);
		CopyAlloc(_Al, (Alloc<T>*)ar._Al);
		return *this;
	}
	int refs()const{return _Al ? _Al->is_used() : 0;}
	size_t count()const{ return _Al ? _Al->count() : 0; }
	size_t memory_usage()const{ return count()*sizeof(T); }
//	size_t memory_total()const{ return count()*sizeof(T); }
	~AllocRef(){
		DeleteAlloc(_Al);
		_Al = 0;
	}
	T *get_new(){
		return _Al->get_new();
	}
	template<class Y>
	T *get_new(const Y& y){
		return _Al->get_new(y);
	}
	template<class Y, class Z>
	T *get_new(const Y& y, const Z &z){
		return _Al->get_new(y, z);
	}
	template<class Y, class Z, class W>
	T *get_new(const Y& y, const Z &z, const W& w){
		return _Al->get_new(y, z, w);
	}
	template<class Y, class Z, class U, class W>
	T *get_new(const Y& y, const Z& z, const U& u, const W& w){
		return _Al->get_new(y, z, u, w);
	}
	template<class Y, class Z, class U, class V, class W>
	T *get_new(const Y& y, const Z& z, const U& u, const V& v, const W& w){
		return _Al->get_new(y, z, u, v, w);
	}
	void free(T *p){
		return _Al->free(p);
	}
};

class ResizableAllocRef{
	ResizableAlloc *_Al;
public:
	void swap(ResizableAllocRef &ar){
		ResizableAlloc *t = _Al; _Al = ar._Al; ar._Al = t;
	}
	ResizableAllocRef(size_t obj_sz = 0, size_t start_sz = 10, double k = 1.1){
		_Al = new ResizableAlloc(obj_sz, start_sz, k);
	}
	void setObjSize(size_t obj_sz){
		if (!_Al)_Al = new ResizableAlloc(obj_sz);
		else if (_Al->is_used() > 1)*this = ResizableAllocRef(obj_sz, _Al->start_size, _Al->exp_coef);
		else _Al->setObjSize(obj_sz);
	}
	ResizableAllocRef(const ResizableAllocRef &ar){
		inc_used(_Al = ar._Al);
	}
	ResizableAllocRef& operator=(const ResizableAllocRef &ar){
		inc_used(ar._Al);
		dec_used(_Al);
		_Al = ar._Al;
		return *this;
	}
	int refs()const{return _Al ? _Al->is_used() : 0;}
	size_t count()const{ return _Al ? _Al->sz : 0; }
	size_t memory_usage()const{ return _Al ? _Al->sz*_Al->obj_size : 0; }
	~ResizableAllocRef(){
		dec_used(_Al);
		_Al = nullptr;
	}
	void *get_new() {
		return _Al->get_new();
	}
    template<class Y>
    Y *get_new() {
        static_assert(sizeof(Y) == _Al->obj_size, "Size of Y must be equal to obj_size");
        static_assert(std::is_standard_layout<Y>::value, "Y must be standard layout");
        static_assert(std::is_trivial<Y>::value, "Y must be trivial");
        return static_cast<Y*>(_Al->get_new());
    }

	void free(void *p){
		return _Al->free(p);
	}
};

#else
class ResizableAllocRef{
	size_t obj_size;
	size_t _cnt;
public:
	void swap(ResizableAllocRef &ar){
		size_t t = obj_size; obj_size = ar.obj_size; ar.obj_size = t;
		t = _cnt; _cnt = ar._cnt; ar._cnt = t;
	}
	ResizableAllocRef(size_t obj_sz = 0, size_t = 10, double = 1.1){
		obj_size = obj_sz; _cnt = 0;
	}
	void setObjSize(size_t obj_sz){
		obj_size = obj_sz;
	}
	int refs()const{return 1;}
	char *get_new(){
		_cnt++;
		return (char*)malloc(obj_size);
	}
	void free(void *p){
		_cnt--;
		::free(p);
	}
	size_t count()const{ return _cnt; }
};

template<class T>
class AllocRef{
	size_t _cnt;
public:
	void swap(AllocRef<T> &){}
	AllocRef(size_t start_sz = 10, double k = 1.5){ (void)start_sz; (void)k; _cnt = 0; }
	AllocRef(const AllocRef<T> &){}
	AllocRef& operator=(const AllocRef &r){
		_cnt = r._cnt;
		return *this;
	}
	int refs()const{return 0;}
	~AllocRef(){}
	T *get_new(){
		_cnt++;
		return new T;
	}
	template<class Y>
	T *get_new(const Y& y){
		_cnt++;
		return new T(y);
	}
	template<class Y, class Z>
	T *get_new(const Y& y, const Z &z){
		_cnt++;
		return new T(y, z);
	}
	template<class Y, class Z, class W>
	T *get_new(const Y& y, const Z &z, const W& w){
		_cnt++;
		return new T(y, z, w);
	}
	template<class Y, class Z, class U, class W>
	T *get_new(const Y& y, const Z& z, const U& u, const W& w){
		_cnt++;
		return new T(y, z, u, w);
	}
	template<class Y, class Z, class U, class V, class W>
	T *get_new(const Y& y, const Z& z, const U& u, const V& v, const W& w){
		_cnt++;
		return new T(y, z, u, v, w);
	}
	void free(T *p){
		_cnt--;
		delete p;
	}
	size_t count()const{ return _cnt; }
};
#endif
template<class T>
void Exchange(AllocRef<T> &x, AllocRef<T> &y){x.swap(y);}
