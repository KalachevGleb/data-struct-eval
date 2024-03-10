#pragma once
namespace _TempSpace{
class _RefBase;
class _CRefBase2;
class _RefBase2;
}

/**
 * @class ReferencedBase
 * @brief
 * Класс ReferencedBase используется как
 * счетчик ссылок.
 *
 * Может быть полезен,
 * когда есть много ссылок
 * на один и тот же объект и неизвестно,
 * в каком порядке они будут удаляться.
 * Счетчик ссылок удаляется автоматически,
 * когда количество ссылок на него становится равным 0.
 * Этот класс используется совместно с классами
 * ClassRef и ClassConstRef.
 * @code
#include "classref.h"

struct a : public ReferencedBase{
	int *p;
	a(int size, int *data){
		p = new int[size];
		for(int i=0; i<size; i++)
			p[i] = data[i];
	}
	~a(){
		delete[] p;
	}
};

typedef ClassRef<a> A;

A createArray(int *array, int length){
	return new a(length);
}
@endcode
 */
class ReferencedBase{
	friend class _TempSpace::_RefBase;
	mutable int _used;
	void _inc_used()const{if(this)++_used;}
	void _dec_used()const{
		if(this && !--_used)
			delete this;
	}
protected:
	/// @return count of references to current object
	int used()const{return _used;}
	ReferencedBase(){_used = 0;}
//public:
	virtual ~ReferencedBase(){}
};

class ReferencedBase2{
	friend class _TempSpace::_CRefBase2;
	friend class _TempSpace::_RefBase2;
	mutable int _used, _cused;
	void _inc_used()const{if(this)++_used;}
	void _inc_cused()const{if(this)++_cused;}
	void _dec_used()const{
		if(this && !--_used&&!_cused)
			delete this;
	}
	void _dec_cused()const{
		if(this && !--_cused && !_used)
			delete this;
	}
protected:
	int used()const{return _used;}
	int constUsed()const{return _cused;}
	ReferencedBase2(){_used = 0;_cused=0;}
//public:
	virtual ~ReferencedBase2(){}
};

namespace _TempSpace{
class _RefBase{
protected:
	_RefBase(){}
	_RefBase(const ReferencedBase *x){x->_inc_used();}
	void _inc(const ReferencedBase *x)const{x->_inc_used();}
	void _dec(const ReferencedBase *x)const{x->_dec_used();}
};

class _RefBase2{
protected:
	_RefBase2(){}
	_RefBase2(const ReferencedBase2 *x, bool c){if(c)x->_inc_cused();else x->_inc_used();}
	void _cinc(const ReferencedBase2 *x)const{x->_inc_cused();}
	void _cdec(const ReferencedBase2 *x)const{x->_dec_cused();}
	void _inc(const ReferencedBase2 *x)const{x->_inc_used();}
	void _dec(const ReferencedBase2 *x)const{x->_dec_used();}
};
}
template<class T> class ClassRef;
template<class T> class ClassRef2;

/**
 * @brief Класс ClassConstRef<T> представляет указатель на
 *класс, который должен наследоваться от класса ReferencedBase.
 *
 *Этот класс заменяет указатель типа const T*, при этом обеспечивая
 *корректный подсчет ссылок на заданный объект, то есть при создании
 *нового экземпляра ClassConstRef, указывающего на объект x, счетчик
 *ссылок объекта x увеличивается на 1; при удалении этой ссылки счетчик
 *уменьшается на 1, и если его значение стало равно 0, то объект x удаляется.
 *
 */
template<class T>
class ClassConstRef : protected _TempSpace::_RefBase{
	typedef _TempSpace::_RefBase _base;
protected:
	T *ref;
public:
	/// @name Конструкторы
	/// @{
	ClassConstRef(T*x=0):_base(x){ref=x;}
	ClassConstRef(const ClassConstRef<T>& r):_base((T*)r.ref){ref = (T*)r.ref;}
	/// @}
	/// Деструктор, уменьшает счетчик ссылок на 1.
	~ClassConstRef(){this->_dec(ref);}
	ClassConstRef<T>& operator=(const ClassConstRef<T>& r){
		if(ref!=r.ref){
			this->_dec(ref);
			this->_inc(ref = (T*)r.ref);
		}
		return *this;
	}
	const T *operator->()const{return ref;}
	const T* pointer()const{return ref;}
	const T& operator*()const{return *ref;}
	const T& reference()const{return *ref;}
	bool operator ==(const ClassConstRef<T>& r)const{return ref == r.ref;}
	bool operator !=(const ClassConstRef<T>& r)const{return ref != r.ref;}
	bool isValid()const{return ref!=0;}
	bool operator!(){return !ref;}
	ClassRef<T> copy()const;
};

template<class T>
class ClassRef:public ClassConstRef<T>{
public:
	ClassRef(T*x=0):ClassConstRef<T>(x){}
	ClassRef(const ClassRef<T>& r):ClassConstRef<T>(r){}
	ClassRef<T>& operator=(const ClassRef<T>& r){
		ClassConstRef<T>::operator =(r);
		return *this;
	}
	T* operator->()const{return (T*)this->ref;}
	T* pointer()const{return (T*)this->ref;}
	T* const& pointerref()const{return *(T**)&this->ref;}
	T& operator*()const{return *(T*)this->ref;}
	T& reference()const{return *(T*)this->ref;}
	~ClassRef(){}
};

template<class T>
ClassRef<T> ClassConstRef<T>::copy()const{
	if(ref)return new T(*ref);
	return ClassRef<T>();
}

template<class T>
class ClassConstRef2 : protected _TempSpace::_RefBase2{
	typedef _TempSpace::_RefBase2 _base;
protected:
	mutable T *ref;
	ClassConstRef2(T*x, bool c):_base(x,c){ref=x;}
public:
	ClassConstRef2(T*x=0):_base(x,true){ref=x;}
	ClassConstRef2(const ClassConstRef2<T>& r):_base((T*)r.ref){ref = (T*)r.ref;}
	ClassConstRef2<T>& operator=(const ClassConstRef2<T>& r){
		this->_cinc(r.ref);
		this->_cdec(ref);
		ref = r.ref;
		return *this;
	}
	const T *operator->()const{return ref;}
	const T* pointer()const{return ref;}
	const T& operator*()const{return *ref;}
	const T& reference()const{return *ref;}
	bool operator ==(const ClassConstRef2<T>& r)const{return ref == r.ref;}
	bool operator !=(const ClassConstRef2<T>& r)const{return ref != r.ref;}
	bool isValid()const{return ref!=0;}
	bool operator!(){return !ref;}
	ClassRef2<T> copy()const;
	~ClassConstRef2(){this->_cdec(ref);ref=0;}
};
template<class T>
class ClassRef2:public ClassConstRef2<T>{
public:
	ClassRef2(T*x=0):ClassConstRef2<T>(x,false){}
	ClassRef2(const ClassRef2<T>& r):ClassConstRef2<T>(r.ref,false){}
	ClassRef2<T>& operator=(const ClassRef2<T>& r){
		this->_inc(r.ref);
		this->_dec(this->ref);
		this->ref = r.ref;
		return *this;
	}
	T* operator->()const{return (T*)this->ref;}
	T* pointer()const{return (T*)this->ref;}
	T& operator*()const{return *(T*)this->ref;}
	T& reference()const{return *(T*)this->ref;}
	~ClassRef2(){this->_dec(this->ref); this->ref = 0;}
};

template<class T>
ClassRef2<T> ClassConstRef2<T>::copy() const{
	if(ref)return new T(*ref);
	return ClassRef2<T>();
}

#define DECLARE_CLASS_REFS(class_name)\
	typedef ClassConstRef<class_name> class_name##ConstRef; \
	typedef ClassRef<class_name> class_name##Ref;

#define DECLARE_FRIEND_REFS(class_name)\
	friend class ClassConstRef<class_name>; \
	friend class ClassRef<class_name>;

