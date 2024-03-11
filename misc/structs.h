#pragma once
#ifndef HASH_T_H
#define HASH_T_H

#include <stdio.h>
#include "HashFunctions.h"
#include "item_memory.cpp"
#include "Stack.h"

#define U_HASH_DEFSIZE 10000

#define LL_DELETEALL	-1

#define IHASH_MAX_C				20
#define UHASH_NOTINIT			-1

#define FRF_FIND						1
#define FRF_ONLYEXISTING	2

#define FRF_DONOTHING		0
#define FRF_REPLACE				4
#define FRF_ADDBEFORE		8
#define FRF_ADDAFTER			12
#define FRF_ADDTODATA		16
#define FRF_DOIFEXIST			28

#define UHASH_FRF_FIND 						FRF_FIND
#define UHASH_FRF_ONLYEXISTING 	FRF_ONLYEXISTING

#define UHASH_FRF_DONOTHING 		FRF_DONOTHING
#define UHASH_FRF_REPLACE 			FRF_REPLACE
#define UHASH_FRF_ADDBEFORE 		FRF_ADDBEFORE
#define UHASH_FRF_ADDAFTER 		FRF_ADDAFTER
#define UHASH_FRF_DOIFEXIST 		FRF_DOIFEXIST

#define UHASH_DELETEALL 	LL_DELETEALL

////////////////////////////////////////////////////////
class Std_Struct;
template <typename TD, typename TK> class StdStruct;
template <typename TD, typename TK> class SLL_item;
template <typename TD, typename TK> class SLL_KA_item;
template <typename TD, typename TK> class LL_item;
template <typename TD, typename TK> class LL_KA_item;
template <typename TD, typename TK, class LLI> class L_List_LT;
template <typename TD, typename TK, class LLI> class SL_List_LT;
template <typename TD, typename TK, class LLI> class DL_List_LT;
template <typename TD, typename TK> class L_List;
template <typename TD, typename TK> class L_KA_List;
template <typename TD, typename TK> class L_List_f;
template <typename TD, typename TK> class L_KA_List_f;
template <typename TD, typename TK> class Hash;
template <typename TD, typename TK> class KC_Hash;
template <typename TD, typename TK> class KA_Hash;
template <typename TD, typename TK, class LL> class U_LT_Hash;
template <typename TD, typename TK, class LL> class U_LTKA_Hash;
template <typename TD, typename TK> class U_Hash;
template <typename TD, typename TK> class U_KA_Hash;
template <typename TD, typename TK> class Tree_item;
template <typename TD, typename TK> class TreeItem_KA;
template <typename TD, typename TK> class Tree;
template <typename TD, typename TK, class TI> class BinaryTree;
template <typename TD, typename TK> class Binary_Tree;
template <typename TD, typename TK> class BinaryTree_KA;
template <typename TD, typename TK> class RB_Tree;
template <typename TD, typename TK> class RB_Tree_KA;
////////////////////////////////////////////////////////
enum stdstr_type{
	STP_RBTREE,
	STP_UHASH,
	STP_BINARYTREE,
//	STP_IHASH,
//	STP_SLINKEDLIST,
	STP_DLINKEDLIST
};
//______________________________________________________
struct ss_alloc_except
{
	string Where;
	size_t size;
	ss_alloc_except(const char * _Where, size_t Size) : Where(), size(Size){Where=_Where;}
	ss_alloc_except() : Where(), size(0){}
};
///======= BASE CLASS ========//
class Std_Struct : public err_outp<Std_Struct>
{
protected:
	int ne;
public:
	int num_elements() const {return ne;}
	Std_Struct(){ne=0;}
	virtual int insert(const void *key, const void *data, unsigned int FR_flag = FRF_FIND | FRF_REPLACE)=0;
	virtual void *insert_adress(const void *key, const void *data, unsigned int FR_flag = FRF_FIND | FRF_REPLACE)=0;
	virtual int _delete(const void *key, int N = LL_DELETEALL)=0;
	virtual int print_v(FILE*, void*) const{return 0;}
	virtual void* get(const void *key)=0;
	virtual const void* get_key(const void *key) const =0;
	virtual Std_Struct& operator=(const Std_Struct&){return *this;}
	virtual ~Std_Struct(){}
};
//______________________________________________________
///======= BASE TEMPLATE STRUCT ========//
template <typename TD,typename TK>
class StdStruct : public Std_Struct
{
public:
	StdStruct(){}
	virtual int insert(const TK &key, const TD *data,unsigned int FR_flag = FRF_FIND | FRF_REPLACE)=0;
	virtual int insert(const void *key, const void *data,unsigned int FR_flag = FRF_FIND | FRF_REPLACE)
	{
		if(!key)return -1;
		return insert(*(const TK*)key,(const TD*)data, FR_flag);
	}
	virtual TD* insert_adress(const TK &key, const TD &data, unsigned int FR_flag = FRF_FIND | FRF_REPLACE)=0;
	virtual void* insert_adress(const void *key, const void *data, unsigned int FR_flag = FRF_FIND | FRF_REPLACE)
	{
		if((key)&&(data)) return insert_adress(*(const TK*)key, *(const TD*)data, FR_flag);
		return NULL;
	}
	virtual int _delete(const TK &key, int N = LL_DELETEALL)=0;
	virtual int _delete(const void *key, int N = LL_DELETEALL)
	{
		if(!key)return -1;
		return _delete(*(const TK*)key,N);
	}
	virtual int print(FILE* fp, int (*Print)(FILE*, const TK&, const TD&))const{return 0;}
	virtual int print_v(FILE* fp, void *Print) const {return print(fp, (int (_cdecl *)(FILE*, const TK&, const TD&))Print);}
	virtual TD* get(const TK& key)=0;
	virtual void* get(const void *key)
	{
		if(!key)return NULL;
		return (void*)get(*(const TK*)key);
	}
	virtual const void* get_key(const void *key)const
	{
		if(!key)return NULL;
		return get_key(*(TK*)key);
	}
	virtual const TK* get_key(const TK &key) const =0;
	virtual StdStruct<TD,TK>& operator=(const StdStruct<TD,TK>&){return *this;}
	virtual ~StdStruct(){}
};

//______________________________________________________
///=====================================///
///======== LINKED LISTS =================//
///====singly linked lists ============//
template <typename TD, typename TK>
class SLL_item : public err_outp<SLL_item<TD, TK> > 
{
protected:
	SLL_item<TD,TK> *next;
	TK key;
	TD data;
	const static SLL_item<TD,TK> *nil;
	const static SLL_item<TD,TK> Nil;
//	friend class SL_List<TD,TK>;
	friend class item_memory<SLL_item<TD,TK> >;
	friend class memory_block<SLL_item<TD,TK> >;
//	friend class SL_List_f<TD,TK>;
public:
	SLL_item();
	SLL_item(TK, const TD&);
	SLL_item(TK, const TD*, SLL_item<TD,TK> * Prev=NULL);
	SLL_item(const SLL_item<TD,TK>&);
	SLL_item<TD,TK>& operator=(const SLL_item<TD,TK>&);
	~SLL_item();
};
//============================//
//============================//
template <typename TD, typename TK>
class SLL_KA_item : public err_outp<SLL_KA_item<TD, TK> > 
{
protected:
	SLL_KA_item<TD,TK> *next;
	TK key;
	TD data;
	const static SLL_KA_item<TD,TK> *nil;
	const static SLL_KA_item<TD,TK> Nil;
//	friend class SL_KA_List<TD,TK>;
	friend class item_memory<SLL_KA_item<TD,TK> >;
	friend class memory_block<SLL_KA_item<TD,TK> >;
//	friend class SL_KA_List_f<TD,TK>;
public:
	SLL_KA_item();
	SLL_KA_item(const TK&, const TD&);
	SLL_KA_item(const TK&, const TD*, SLL_KA_item<TD,TK> * Prev=NULL);
	SLL_KA_item(const SLL_KA_item<TD,TK>&);
	SLL_KA_item<TD,TK>& operator=(const SLL_KA_item<TD,TK>&);
	~SLL_KA_item();
};

//============================//
//============================//
template <typename TD, typename TK, class LLI>
class L_List_LT : public StdStruct<TD, TK>
{
protected:
	LLI * L, * last;
public:
	L_List_LT();
	L_List_LT(const L_List_LT<TD,TK,LLI>&);
	L_List_LT<TD,TK,LLI>& operator=(const L_List_LT<TD,TK,LLI>&);
	virtual TD * get(const TK&);
	virtual int get(const TK*, TD&, int num=0)const ;
	virtual const TK* get_key(const TK &key) const;
	void clear();
	virtual ~L_List_LT();
	virtual int print(FILE *fp, int (*PrintKD)(FILE*, const TK&, const TD&))const;
};
//============================//
//============================//
template <typename TD, typename TK, class LLI>
class SL_List_LT : public L_List_LT<TD, TK,LLI>
{
public:
	SL_List_LT() : L_List_LT() {}
	SL_List_LT(const SL_List_LT<TD,TK,LLI>&SL) : L_List_LT(SL){}
	SL_List_LT<TD,TK,LLI>& operator=(const SL_List_LT<TD,TK,LLI>&);
	int insert(const TK*, const TD&, unsigned int FR_flag = (UHASH_FRF_FIND | UHASH_FRF_REPLACE));
	virtual int insert(const TK&, const TD*, unsigned int FR_flag);
	virtual TD* insert_adress(const TK&, const TD&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual TD* insert_adress(const TK&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual int delete_(TK*, int num=LL_DELETEALL);
	virtual int _delete(const TK&, int N);
//	virtual const TK* get_key(const TK &key) const;
	virtual ~SL_List_LT(){}
};
//============================//
//============================//
template <typename TD, typename TK, class LLI>
class DL_List_LT : public L_List_LT<TD, TK,LLI>
{
public:
	DL_List_LT() : L_List_LT() {}
	DL_List_LT(const DL_List_LT<TD,TK,LLI>&DL) : L_List_LT(DL) {}
	DL_List_LT<TD,TK,LLI>& operator=(const DL_List_LT<TD,TK,LLI>&);
	int insert(const TK*, const TD&, unsigned int FR_flag = (UHASH_FRF_FIND | UHASH_FRF_REPLACE));
	virtual int insert(const TK&, const TD*, unsigned int FR_flag);
	virtual TD* insert_adress(const TK&, const TD&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual TD* insert_adress(const TK&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual int delete_(const TK*, int num=LL_DELETEALL);
	virtual int _delete(const TK&, int N);
//	virtual const TK* get_key(const TK &key) const;
	virtual ~DL_List_LT(){}
};
//============================//
//============================//
template <typename TD, typename TK>
class SL_List : public SL_List_LT<TD, TK, SLL_item<TD, TK> >
{
public:
	SL_List();
	SL_List(const SL_List<TD,TK>&);
	SL_List<TD,TK>& operator=(const SL_List<TD,TK>&);
	int insert(TK, const TD&,unsigned int FR_flag = (UHASH_FRF_FIND | UHASH_FRF_REPLACE));
	virtual int insert(const TK&, const TD*,unsigned int FR_flag);
	TD* operator[](TK);
	virtual TD* get(const TK&);
	int get(TK, TD&,int num=0)const ;
	int delete_(TK, int num=LL_DELETEALL);
	const TK* get_key(const TK &key) const;
//	virtual int _delete(const TK&, int N);
//	void clear();
	int print(FILE *fp, int (*PrintKD)(FILE*, const TK&, const TD&))const;
	virtual ~SL_List();
};
//============================//
//============================//
template <typename TD, typename TK>
class SL_KA_List : public SL_List_LT<TD, TK, SLL_KA_item<TD, TK> >
{
public:
	SL_KA_List();
	SL_KA_List(const SL_KA_List<TD,TK>&);
	SL_KA_List<TD,TK>& operator=(const SL_KA_List<TD,TK>&);
	int insert(const TK&, const TD&, unsigned int FR_flag = (UHASH_FRF_FIND | UHASH_FRF_REPLACE));
	virtual int insert(const TK&, const TD*, unsigned int FR_flag);
	TD* operator[](const TK&);
	int get(const TK&, TD&,int num=0) const;
	virtual TD* get(const TK&);
//	const TK* get_key(const TK &key) const;
	int delete_(const TK&, int num=LL_DELETEALL);
//	virtual int _delete(const TK&, int num);
//	void clear();
	int print(FILE *fp, int (*PrintKD)(FILE*, const TK&, const TD&))const ;
	virtual ~SL_KA_List();
};
///====doubly linked lists ============//
template <typename TD, typename TK>
class LL_item : public err_outp<LL_item<TD, TK> > 
{
protected:
	LL_item<TD,TK> *prev, *next;
	TK key;
	TD data;
	static const LL_item<TD,TK> *nil;
	static const LL_item<TD,TK> Nil;
	friend class L_List<TD,TK>;
	friend class item_memory<LL_item<TD,TK> >;
	friend class memory_block<LL_item<TD,TK> >;
	friend class L_List_f<TD,TK>;
	friend class L_List_LT<TD, TK, LL_item<TD, TK> >;
	friend class SL_List_LT<TD, TK, LL_item<TD, TK> >;
	friend class DL_List_LT<TD, TK, LL_item<TD, TK> >;
public:
	LL_item();
	LL_item(TK, const TD&);
	LL_item(TK, const TD*, LL_item<TD,TK> * Prev = NULL);
	LL_item(const LL_item<TD,TK>&);
	LL_item<TD,TK>& operator=(const LL_item<TD,TK>&);
	~LL_item();
};

/*//______________________________________________________
////////////////////////////////////////////////////////
template <typename TD, typename TK>
class LL_item_f
{
protected:
	LL_item_f<TD,TK> *prev, *next;
	TK key;
	TD data;
	static LL_item_f<TD,TK> *nil;
	friend class L_List_f<TD,TK>;
public:
	LL_item_f();
	LL_item_f(TK,TD&);
	LL_item_f(TK,TD*);
	LL_item_f(LL_item<TD,TK>&);
	LL_item_f<TD,TK>& operator=(LL_item_f<TD,TK>&);
	~LL_item_f();
};*/

////////////////////////////////////////////////////////
//______________________________________________________
template <typename TD, typename TK>
class LL_KA_item : public err_outp<LL_KA_item<TD, TK> > 
{
protected:
	LL_KA_item<TD,TK> *prev, *next;
	TK key;
	TD data;
	static const LL_KA_item<TD,TK> *nil;
	static const LL_KA_item<TD,TK> Nil;
	friend class L_KA_List<TD,TK>;
	friend class item_memory<LL_KA_item<TD,TK> >;
	friend class memory_block<LL_KA_item<TD,TK> >;
	friend class L_KA_List_f<TD,TK>;
	friend class L_List_LT<TD, TK, LL_KA_item<TD, TK> >;
	friend class SL_List_LT<TD, TK, LL_KA_item<TD, TK> >;
	friend class DL_List_LT<TD, TK, LL_KA_item<TD, TK> >;
public:
	LL_KA_item();
	LL_KA_item(const TK&, const TD&);
	LL_KA_item(const TK&, const TD*, LL_KA_item<TD, TK> * Prev=NULL);
	LL_KA_item(const LL_KA_item<TD,TK>&);
	LL_KA_item<TD,TK>& operator=(const LL_KA_item<TD,TK>&);
	~LL_KA_item();
};

//============================//
//============================//
template <typename TD, typename TK>
class L_List : public DL_List_LT<TD, TK, LL_item<TD, TK> >
{
public:
	L_List() : DL_List_LT(){}
	L_List(const L_List<TD,TK> & LL) : DL_List_LT(LL){}
	L_List<TD,TK>& operator=(const L_List<TD,TK>&);
	int insert(TK, const TD&,unsigned int FR_flag = (UHASH_FRF_FIND | UHASH_FRF_REPLACE));
	virtual int insert(const TK&, const TD*,unsigned int FR_flag);
	TD* operator[](TK);
	virtual TD* get(const TK&);
	int get(TK, TD&,int num=0)const ;
	int delete_(TK, int num=LL_DELETEALL);
//	virtual int _delete(const TK&, int N);
//	void clear();
	virtual ~L_List(){}
};
//============================//
//============================//
template <typename TD, typename TK>
class L_List_f : public DL_List_LT<TD, TK, LL_item<TD,TK> >//StdStruct<TD, TK>
{
protected:
	item_memory<LL_item<TD, TK> > im;
public:
	L_List_f() : DL_List_LT() {}
	L_List_f(const L_List_f<TD,TK>&);
	L_List_f<TD,TK>& operator=(const L_List_f<TD,TK>&);
	int insert(TK, const TD&,unsigned int FR_flag = (UHASH_FRF_FIND | UHASH_FRF_REPLACE));
	virtual int insert(const TK&, const TD*,unsigned int FR_flag);
	virtual TD* insert_adress(const TK&, const TD&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual TD* insert_adress(const TK&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	TD* operator[](TK);
	TD* get(const TK&);
	int get(TK, TD&,int num=0)const;
	virtual const TK* get_key(const TK &key) const;
	int delete_(TK, int num=LL_DELETEALL);
	virtual int _delete(const TK&, int N=LL_DELETEALL);
	void clear();
	virtual ~L_List_f();
};
//============================//
//============================//
template <class TD, class TK>
class L_KA_List_f : public DL_List_LT<TD, TK, LL_KA_item<TD, TK> >
{
protected:
	item_memory<LL_KA_item<TD,TK> > im;
public:
	L_KA_List_f();
	L_KA_List_f(const L_KA_List_f<TD,TK>&);
	L_KA_List_f<TD,TK>& operator=(const L_KA_List_f<TD,TK>&);
	int insert(const TK&, const TD&,unsigned int FR_flag = (FRF_FIND | FRF_REPLACE));
	virtual int insert(const TK&, const TD*,unsigned int FR_flag);
	virtual TD* insert_adress(const TK&, const TD&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual TD* insert_adress(const TK&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	TD* operator[](const TK&);
	TD* get(const TK&);
	virtual const TK* get_key(const TK &key) const;
	int get(const TK&, TD&,int num=0) const;
	int delete_(const TK&, int num=LL_DELETEALL);
	virtual int _delete(const TK&, int N=LL_DELETEALL);
	void clear();
	virtual ~L_KA_List_f();
};
//template <class TD, class TK>
//item_memory<LL_KA_item<TD, TK> > L_KA_List_f<TD, TK>::im;
//============================//
//============================//
template <typename TD, typename TK>
class L_KA_List : public DL_List_LT<TD, TK, LL_KA_item<TD, TK> >
{
public:
	L_KA_List() : DL_List_LT(){}
	L_KA_List(const L_KA_List<TD,TK>&LL) : DL_List_LT(LL){}
	L_KA_List<TD,TK>& operator=(const L_KA_List<TD,TK>&);
	int insert(const TK&, const TD&, unsigned int FR_flag = (UHASH_FRF_FIND | UHASH_FRF_REPLACE));
	virtual int insert(const TK&, const TD*, unsigned int FR_flag);
	TD* operator[](const TK&);
	int get(const TK&, TD&,int num=0) const;
	virtual TD* get(const TK&);
	int delete_(const TK&, int num=LL_DELETEALL);
//	virtual int _delete(const TK&, int num);
//	void clear();
	virtual ~L_KA_List();
};
////////////////////////////////////////////////////////
//______________________________________________________
///==================================///
///======== HASHES ==================///
//========== BASE HASH (abstract) ======//
template <typename TD,typename TK>
class Hash : public StdStruct<TD, TK>
{
protected :
	static size_t DefSize;
public: 
	static void SetDefSize(size_t def_size){if(def_size>0) DefSize=def_size;}
	virtual Hash<TD,TK>& operator=(const Hash<TD,TK>&){ return *this;}
	virtual void clear()=0;
	virtual ~Hash(){ne=0;}
};
//==========================================//
///== base universal hash with a numeric key type(abstract) ====//
template <typename TD,typename TK>
class KC_Hash : public Hash<TD,TK>
{
public:
	virtual int insert(TK, const TD&, unsigned int FR_flag=UHASH_FRF_FIND | UHASH_FRF_REPLACE)=0;
	virtual int delete_(TK, int N=UHASH_DELETEALL)=0;
	virtual TD* operator[](TK)=0;
	virtual KC_Hash<TD,TK>& operator=(const KC_Hash<TD,TK>&){return *this;}
	virtual void clear()=0;
	virtual ~KC_Hash(){}
};

//====================================================//
///== base universal hash with a class or a struct as key type(abstract) ====//
template <typename TD,typename TK>
class KA_Hash : public Hash<TD,TK>
{
public:
	virtual int insert(const TK&, const TD&,unsigned int FR_flag=UHASH_FRF_FIND | UHASH_FRF_REPLACE)=0;
	virtual int delete_(const TK&, int N=UHASH_DELETEALL)=0;
	virtual TD* operator[](const TK&)=0;
	virtual KA_Hash<TD,TK>& operator=(const KA_Hash<TD,TK>&){return *this;}
	virtual void clear()=0;
	virtual ~KA_Hash(){}
};


//==============================================//
///== hash with a template parameter with a numeric key type====//
template <typename TD,typename TK,class LL>
class U_LT_Hash : public KC_Hash<TD,TK>
{
protected:
	size_t size;
	LL *P;
	size_t (* Hf)(TK, size_t);
//	static int DefSize;
	static size_t (*DefHf)(TK, size_t);
public:
	static bool SetDefHf(size_t (*def_hf)(TK, size_t));
	bool SetHf(size_t (*def_hf)(TK, size_t));
	U_LT_Hash();
	U_LT_Hash(size_t Size);
	U_LT_Hash(size_t Size, size_t (*hf)(TK, size_t));
	U_LT_Hash(const U_LT_Hash<TD,TK,LL>&);
	virtual ~U_LT_Hash();
	bool init(size_t Size, size_t (*hf)(TK,size_t));
	int insert(TK key, const TD& data, unsigned int FR_flag=UHASH_FRF_FIND | UHASH_FRF_REPLACE);
	virtual int insert(const TK &key, const TD *data,unsigned int FR_flag=UHASH_FRF_FIND | UHASH_FRF_REPLACE);
	virtual TD* insert_adress(const TK&, const TD&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual TD* insert_adress(const TK&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	int delete_(TK, int N);
	virtual int _delete(const TK&, int N);
	TD* operator[](TK);
	virtual TD* get(const TK &key);
	virtual const TK* get_key(const TK &key) const;
	void clear();
	U_LT_Hash<TD,TK,LL>& operator=(const U_LT_Hash<TD,TK,LL>&);
	virtual int print(FILE* fp, int (*Print)(FILE*, const TK&, const TD&)) const;
//	virtual int print(FILE*);
};
//=====================================================//
///== hash with a template parameter with a class or a struct as key type====//
template <typename TD,typename TK,class LL>
class U_LTKA_Hash : public KA_Hash<TD,TK>
{
protected:
	size_t size;
	LL *P;
	size_t (* Hf)(const TK&,size_t);
//	static int DefSize;
	static size_t (* DefHf)(const TK&,size_t);
public:
	static bool SetDefHf(size_t (*def_hf)(const TK&,size_t));
	bool SetHf(size_t (*def_hf)(const TK&,size_t));
	U_LTKA_Hash();
	U_LTKA_Hash(size_t Size);
	U_LTKA_Hash(size_t Size, size_t (*hf)(const TK&,size_t));
	U_LTKA_Hash(const U_LTKA_Hash<TD,TK,LL>&);
	virtual ~U_LTKA_Hash();
	bool init(size_t Size, size_t (*hf)(const TK&,size_t));
	int insert(const TK& key, const TD& data, unsigned int FR_flag=UHASH_FRF_FIND | UHASH_FRF_REPLACE);
	virtual int insert(const TK &key, const TD *data, unsigned int FR_flag=UHASH_FRF_FIND | UHASH_FRF_REPLACE);
	virtual TD* insert_adress(const TK&, const TD&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual TD* insert_adress(const TK&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	int _delete(const TK&,int N);
	virtual int delete_(const TK&,int N);
	TD* operator[](const TK&);
	virtual TD* get(const TK &key);
	const virtual  TK* get_key(const TK &key) const;
	U_LTKA_Hash<TD,TK,LL>& operator=(const U_LTKA_Hash<TD,TK,LL>&);
	virtual int print(FILE* fp, int (*Print)(FILE*, const TK&, const TD&)) const;
	void clear();
};

//================================================//
///====simple universal hashes with a numeric key type=========//
// allocates memory for each new item separately
template <typename TD,typename TK>
class U_Hash : public U_LT_Hash<TD, TK, L_List<TD,TK> >
{
public:
	U_Hash() : U_LT_Hash(){}
	U_Hash(size_t Size) : U_LT_Hash(Size){}
	U_Hash(size_t Size, size_t (*hf)(TK,size_t)) : U_LT_Hash(Size,hf){}
	U_Hash(const U_Hash<TD,TK>& H) : U_LT_Hash(H){}
	int print(FILE*)const ;
//	~U_KA_Hash();
};
///===========================//
// allocates memory for group of items
//(to set size of memory blocks use method im.set_size(int Size))
template <typename TD,typename TK>
class U_Hash_f : public U_LT_Hash<TD, TK, L_List_f<TD,TK> >
{
public:
	U_Hash_f() : U_LT_Hash(){}
	U_Hash_f(size_t Size) : U_LT_Hash(Size){}
	U_Hash_f(size_t Size, size_t (*hf)(const TK,size_t)) : U_LT_Hash(Size,hf){}
	U_Hash_f(const U_Hash_f<TD,TK>& H) : U_LT_Hash(H){}
	int print(FILE*)const ;
//	~U_KA_Hash();
};
///===========================//
// allocates memory for group of items
//(to set size of memory blocks use method im.set_size(int Size))
template <typename TD,typename TK>
class U_KA_Hash_f : public U_LTKA_Hash<TD, TK, L_KA_List_f<TD,TK> >
{
public:
	U_KA_Hash_f() : U_LTKA_Hash(){}
	U_KA_Hash_f(size_t Size) : U_LTKA_Hash(Size){}
	U_KA_Hash_f(size_t Size, size_t (*hf)(const TK&, size_t)) : U_LTKA_Hash(Size, hf){}
	U_KA_Hash_f(const U_KA_Hash_f<TD,TK>& H) : U_LTKA_Hash(H){}
	int print(FILE*)const ;
//	~U_KA_Hash();
};
//======================================================//
///====simple universal hash with a class or a struct as key type=========//
// allocates memory for each new item separately
template <typename TD,typename TK>
class U_KA_Hash : public U_LTKA_Hash<TD, TK, L_KA_List<TD,TK> >
{
public:
	U_KA_Hash() : U_LTKA_Hash(){}
	U_KA_Hash(size_t Size) : U_LTKA_Hash(Size){}
	U_KA_Hash(size_t Size, size_t (*hf)(const TK&,size_t)) : U_LTKA_Hash(Size,hf){}
	U_KA_Hash(const U_KA_Hash<TD,TK>& H) : U_LTKA_Hash(H){}
	int print(FILE*)const ;
//	~U_KA_Hash();
};
//==================================///
///===== ideal hash (with a static key set) =====//
template <typename TD>
struct IH_F
{
	int hf_par1, hf_par2;
	int ne;
	TD *D;
};
//==================================//
template <typename TD,typename TK>
class I_Hash : public Hash<TD, TK>
{
protected:
	int hf_par1,hf_par2;
	int (*h1)(const TK&, int, int, int);
	int (*h2)(const TK&, int, int, int);
	IH_F<TD> *P;
	static int (*DefHf)(const TK&, int, int, int);
public:
	I_Hash();
	I_Hash(int Size, const TK *keys, const TD *values, int (*h1)(const TK&,int,int,int)=NULL, int (*h2)(const TK&,int,int,int)=NULL);
	I_Hash(const I_Hash<TD,TK>& H);
	I_Hash<TD,TK>& operator=(const I_Hash<TD,TK>&);
	void clear();
	void reset(int Size, const TK *keys, const TD *values, int (*h1)(const TK&,int,int,int), int (*h2)(const TK&,int,int,int));
	~I_Hash();
	int print(FILE*)const ;
};
//_______________________________________________________
///====================================///
///============ BINARY TREES===========///
template <typename TD,typename TK>
class Tree_item
{
protected:
	Tree_item<TD,TK> *left,*right,*p;
	TK key;
	TD data;
	friend class Binary_Tree<TD,TK>;
	friend class BinaryTree<TD, TK, Tree_item<TD,TK> >;
	static Tree_item<TD,TK>* nil;
	static Tree_item<TD,TK> Nil;
//----------------------------------------
public:
	static const Tree_item<TD,TK> * NIL();
	Tree_item();
	Tree_item(const Tree_item<TD,TK>&, Tree_item<TD,TK> * P);
	Tree_item(TK, const TD &, Tree_item<TD,TK> * P=NULL);
	Tree_item(TK, const TD *, Tree_item<TD,TK> * P=NULL);
	virtual ~Tree_item();
	virtual int print(int (*printTK)(TK), int (*printTD)(const TD&))const ;
	virtual int print(FILE* fp, int (*Print)(FILE*, const TK&, const TD&)) const;
	virtual void init(TK key, const TD& data);
	virtual Tree_item<TD,TK>& operator=(const Tree_item<TD,TK> &TI);
	TD& get_data();
	TK get_key() const;
};
//_______________________________________________________
template <typename TD,typename TK>
class TreeItem_KA		//TK may be a struct or a class
{
protected:
	TreeItem_KA<TD,TK> * left, * right, * p;
	TK key;
	TD data;
	friend class BinaryTree<TD, TK, TreeItem_KA<TD,TK> >;
	friend class BinaryTree_KA<TD,TK>;
	static TreeItem_KA<TD,TK>* nil;
	static TreeItem_KA<TD,TK> Nil;
//----------------------------------------
public:
	static const TreeItem_KA<TD,TK> *NIL();
	TreeItem_KA();
	TreeItem_KA(const TreeItem_KA<TD,TK>&, TreeItem_KA<TD,TK> *P);
	TreeItem_KA(const TK&, const TD&, TreeItem_KA<TD,TK> *P=NULL);
	TreeItem_KA(const TK&, const TD*, TreeItem_KA<TD,TK> *P=NULL);
	virtual ~TreeItem_KA();
	virtual int print(int (*printTK)(const TK&), int (*printTD)(const TD&)) const;
	virtual int print(FILE* fp, int (*Print)(FILE*, const TK&, const TD&)) const;
	virtual void init(const TK& key, const TD& data);
	virtual TreeItem_KA<TD,TK>& operator=(const TreeItem_KA<TD,TK> &TI);
	TD& get_data();
	const TK& get_key() const;
};
/////////////////////////////////////////////////////////
//-------------------------------------------------------
template <typename TD,typename TK>
class Tree : public StdStruct<TD, TK>
{
public:
	virtual Tree<TD,TK>& operator=(const Tree<TD,TK> &T){return *this;}
	virtual ~Tree();
};
/////////////////////////////////////////////////////////
//-------------------------------------------------------
template <typename TD,typename TK,class TI>
class BinaryTree : public StdStruct<TD, TK>
{
protected:
	TI *root, *nil;
	virtual int left_rotate(TI *);
	virtual int right_rotate(TI *);
public:
	virtual TI* successor(TI *);
	virtual TI* predecessor(TI *);
	inline virtual TI* minimum(TI *);
	inline virtual TI* maximum(TI *);
	TI*& find(const TK&, TI** P=NULL);
	TI*const& find_c(const TK& Key, const TI** P=NULL) const;
	virtual BinaryTree<TD,TK,TI>& operator=(const BinaryTree<TD,TK,TI> &T);
	virtual int insert(const TK&, const TD*, unsigned int FR_flag);
	virtual TD* insert_adress(const TK&, const TD&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual TD* insert_adress(const TK&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual int print(FILE* fp, int (*Print)(FILE*, const TK&, const TD&)) const;
	virtual int _delete(const TK&, int N);
	virtual TD* get(const TK&);
	virtual const TK* get_key(const TK &key) const;
	virtual void clear();
	virtual ~BinaryTree();
};
/////////////////////////////////////////////////////////
//-------------------------------------------------------
template <typename TD,typename TK>
class Binary_Tree : public BinaryTree<TD, TK, Tree_item<TD,TK> >
{
//private:
protected:
	virtual int left_rotate(TK);
	virtual int right_rotate(TK);
	virtual Tree_item<TD,TK>*& Find(TK, Tree_item<TD,TK>** P=NULL);
//----------------------------------------
public:
	Binary_Tree();
	Binary_Tree(const Binary_Tree<TD,TK>&);
	virtual ~Binary_Tree();
//	virtual int print(int (*printTK)(TK), int (*printTD)(const TD&)) const;
	void init(TK key,const TD& data);
	virtual Tree_item<TD,TK>* insert(TK, const TD&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual int insert(const TK &key, const TD *data, unsigned int FR_flag){return BinaryTree::insert(key,data,FR_flag);}
	virtual Tree_item<TD,TK>* delete_(TK);
//	virtual int _delete(TK&, int N);
	virtual inline TD* operator[](TK);
	virtual Binary_Tree<TD,TK>& operator=(const Binary_Tree<TD,TK>&T);
};

//_______________________________________________________
///============================//
///====== RED-BLACK TREES =====//
#define RBT_RED		1
#define RBT_BLACK	2
//=============================//
template <typename TD,typename TK>
class RB_item
{
protected:
	RB_item<TD,TK> *left,*right,*p;
	TK key;
	TD data;
	char color;
	friend class RB_Tree<TD,TK>;
	friend class BinaryTree<TD, TK, RB_item<TD,TK> >;
	static RB_item<TD,TK>* nil;
	static RB_item<TD,TK> Nil;
//----------------------------------------
public:
	static const RB_item<TD,TK> *NIL();
	RB_item();
	RB_item(const RB_item<TD,TK>&, RB_item<TD,TK> *P);
	RB_item(TK, const TD&, char color=RBT_BLACK, RB_item<TD,TK> *P=NULL);
	RB_item(TK, const TD*, RB_item<TD,TK> *P=NULL);
	RB_item(TK, const TD*, char color, RB_item<TD,TK> *P=NULL);
	virtual ~RB_item();
	virtual int print(FILE*, int (*printTK)(FILE*,const TK&), int (*printTD)(FILE*,const TD&))const ;
	virtual int print(FILE* fp, int (*Print)(FILE*, const TK&, const TD&)) const;
	virtual void init(TK key, const TD& data, char Color = RBT_BLACK);
	virtual RB_item<TD,TK>& operator=(const RB_item<TD,TK> &TI);
	TD& get_data();
	TK get_key() const;
};
/////////////////////////////////////////////////////////
//-------------------------------------------------------
template <typename TD,typename TK>
class RB_item_KA
{
protected:
	RB_item_KA<TD,TK> *left,*right,*p;
	TK key;
	TD data;
	char color;
	friend class RB_Tree_KA<TD,TK>;
	friend class BinaryTree<TD, TK, RB_item_KA<TD,TK> >;
	static RB_item_KA<TD,TK>* nil;
	static RB_item_KA<TD,TK> Nil;
//----------------------------------------
public:
	static const RB_item_KA<TD,TK> *NIL();
	RB_item_KA();
	RB_item_KA(const RB_item_KA<TD,TK>&, RB_item_KA<TD,TK> *P);
	RB_item_KA(const TK&, const TD&, char color=RBT_BLACK, RB_item_KA<TD,TK> *P=NULL);
	RB_item_KA(const TK&, const TD*, RB_item_KA<TD,TK> *P=NULL);
	RB_item_KA(const TK&, const TD*, char color, RB_item_KA<TD,TK> *P=NULL);
	virtual ~RB_item_KA();
	virtual int print(FILE*, int (*printTK)(FILE*, const TK&), int (*printTD)(FILE*, const TD&))const ;
	virtual int print(FILE* fp, int (*Print)(FILE*, const TK&, const TD&)) const;
	virtual void init(const TK& key, const TD& data, char Color = RBT_BLACK);
	virtual RB_item_KA<TD,TK>& operator=(const RB_item_KA<TD,TK> &TI);
	TD& get_data();
	const TK& get_key() const;
};
/////////////////////////////////////////////////////////
//-------------------------------------------------------
template <typename TD,typename TK>
class BinaryTree_KA : public BinaryTree<TD,TK,TreeItem_KA<TD,TK> >
{
protected:
	int left_rotate(const TK&);
	int right_rotate(const TK&);
	friend TreeItem_KA<TD,TK>** Find(const TK&);
	TreeItem_KA<TD,TK>*& Find(const TK&, TreeItem_KA<TD,TK>** P=NULL);
//	static BinaryTree_KA* nil;
//----------------------------------------
public:
	BinaryTree_KA();
	BinaryTree_KA(const BinaryTree_KA<TD,TK>&);
	virtual ~BinaryTree_KA();
//	virtual int print(int (*printTK)(const TK&), int (*printTD)(const TD&)) const;
//	virtual void init(const TK& key, const TD& data);
	virtual TreeItem_KA<TD,TK>* insert(const TK&, const TD&,unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual int insert(const TK &key, const TD *data, unsigned int FR_flag){return BinaryTree::insert(key,data,FR_flag);}
	virtual TreeItem_KA<TD,TK>* delete_(const TK&);
//	TD& exclude(const TK&);
	virtual TD* operator[](const TK&);
//	virtual const TK* get_key(const TK &key) const;
	virtual BinaryTree_KA<TD,TK>& operator=(const BinaryTree_KA<TD,TK>&T);
};

/////////////////////////////////////////////////////////
//-------------------------------------------------------
template <typename TD,typename TK>
class RB_Tree : public BinaryTree<TD, TK, RB_item<TD,TK> >
{
protected:
	RB_item<TD,TK>*& Find(TK, RB_item<TD,TK>** P=NULL);
	void delete_fixup(RB_item<TD,TK> *Z);
	void insert_fixup(RB_item<TD,TK>*);
//----------------------------------------
public:
	RB_Tree();// : Binary_Tree<TD,TK>(),nil(RB_item<TD,TK>::nil){};
	RB_Tree(const RB_Tree<TD,TK>& T);// : Binary_Tree<TD,TK>(T){};
	virtual ~RB_Tree();
//	int print(FILE*);
//	int print(int (*printTK)(TK), int (*printTD)(TD&));
//	void init_func(int Cmp(TK,TK), int CopyDel(TK&,TK));
//	void init(TK key,TD& data);
	RB_item<TD,TK>& insert(TK, const TD&, unsigned int FR_flag);
	RB_item<TD,TK>* delete_(TK);
	virtual int insert(const TK&, const TD*, unsigned int FR_flag);
	virtual TD* insert_adress(const TK&, const TD&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual int _delete(const TK&, int N);
	TD* operator[](TK);
	RB_Tree<TD,TK>& operator=(const RB_Tree<TD,TK>&T){BinaryTree<TD, TK, RB_item<TD, TK> >::operator =(T); return *this;}
};
//_______________________________________________________
//---------------------------------
template <typename TD,typename TK>
class RB_Tree_KA : public BinaryTree<TD, TK, RB_item_KA<TD,TK> >
{
protected:
	RB_item_KA<TD,TK>*& Find(const TK&, RB_item_KA<TD,TK>** P=NULL);
	void delete_fixup(RB_item_KA<TD,TK> *Z);
	void insert_fixup(RB_item_KA<TD,TK>*);
//----------------------------------------
public:
	RB_Tree_KA();// : Binary_Tree<TD,TK>(),nil(RB_item_KA<TD,TK>::nil){};
	RB_Tree_KA(const RB_Tree_KA<TD,TK>& T);// : Binary_Tree<TD,TK>(T){};
	virtual ~RB_Tree_KA();
//	int print(FILE*) const;
//	int print(int (*printTK)(const TK&), int (*printTD)(TD&));
//	void init_func(int Cmp(const TK&,TK), int CopyDel(const TK&&,TK));
//	void init(const TK& key,TD& data);
	RB_item_KA<TD,TK>& insert(const TK&, const TD&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	RB_item_KA<TD,TK>* delete_(const TK&);
	virtual int insert(const TK&, const TD*, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual TD* insert_adress(const TK&, const TD&, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual TD* insert_adress(const TK& Key, unsigned int FR_flag=FRF_FIND | FRF_REPLACE);
	virtual int _delete(const TK&, int N);
	TD* operator[](const TK&);
	RB_Tree_KA<TD,TK>& operator=(const RB_Tree_KA<TD,TK>&T){BinaryTree<TD, TK, RB_item_KA<TD, TK> >::operator =(T); return *this;}
};
///==================================//
///======== METHODS ================//
///==================================//
#define T_DELETE(x) if(x!=nil){delete x;  x=nil;}
#define CMP_NUM(a,b) ((a)>(b) ? 1 : ((a)<(b) ? -1 : 0))
///=======CLASS U_LT_Hash==========//
template <typename TD, typename TK>
size_t Hash<TD,TK>::DefSize=U_HASH_DEFSIZE;
//--------------------------------------------
template <typename TD, typename TK, class LL>
size_t (* U_LT_Hash<TD,TK,LL>::DefHf)(TK, size_t)=NULL;
//--------------------------------------------
template <typename TD, typename TK, class LL>
bool U_LT_Hash<TD,TK,LL>::SetDefHf(size_t (*def_hf)(TK,size_t))
{
	if((DefHf)&&(!def_hf))return false;
	DefHf=def_hf;
	return true;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
bool U_LT_Hash<TD,TK,LL>::SetHf(size_t (*def_hf)(TK,size_t))
{
	if(((Hf)&&(ne))||(!def_hf))return false;
	Hf=def_hf;
	return true;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
U_LT_Hash<TD,TK,LL>::U_LT_Hash()
{
	if(DefHf) Hf=DefHf;
	else Hf=NULL;
	if(DefSize>0) 
	{
		try{ P=new LL[DefSize]; }
		catch(...){ P=NULL; }
		if(!P)
		{
			OUT_ERROR("no enough memory for Hash; size=%d\n", DefSize);
			ne=size=0;
			throw ss_alloc_except("constructor U_LT_Hash()", DefSize*sizeof(LL));
		}
		size=DefSize;
	}
	else {P=NULL; size=0;}
	ne=0;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
U_LT_Hash<TD,TK,LL>::U_LT_Hash(size_t Size)
{
	if(Size>0)size=Size;
	else size=(DefSize>0 ? DefSize : 0);
	if(size>0)
	{
		try{ P=new LL[size]; }
		catch(...) { P=NULL; }
		if(!P)
		{
			OUT_ERROR("no enough memory for Hash; size=%d\n", size);
			ne=size=0;
			throw ss_alloc_except("constructor U_LT_Hash(int)", Size*sizeof(LL));
		}
	}
	else P=NULL;
	Hf=DefHf;
	ne=0;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
U_LT_Hash<TD,TK,LL>::U_LT_Hash(size_t Size, size_t (*hf)(TK,size_t))
{
	if(Size>0)size=Size;
	else size=(DefSize>0 ? DefSize : 0);
	if(size>0)
	{
		try{ P=new LL[size]; }
		catch(...) { P=NULL; }
		if(!P)
		{
			OUT_ERROR("no enough memory for Hash; size=%d (%d bytes)\n", size, size*sizeof(LL));
			ne=size=0;
			throw ss_alloc_except("constructor U_LT_Hash(int, int (*)(TK, int))", Size*sizeof(LL));
		}
	}
	else P=NULL;
	Hf=hf;
	ne=0;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
U_LT_Hash<TD,TK,LL>::U_LT_Hash(const U_LT_Hash<TD,TK,LL>& H)
{
	size=H.size;
	if(size>0)
	{
		try{ P=new LL[size]; }
		catch(...) { P=NULL; }
		if(!P)
		{
			OUT_ERROR("no enough memory for Hash; size=%d\n", size);
			ne=size=0;
			throw ss_alloc_except("constructor U_LT_Hash(const U_LT_Hash &)", H.size*sizeof(LL));
		}
	}
	else P=NULL;
	if((P)&&(H.P))
	{
		for(size_t i=0; i<size; i++)
			P[i]=H.P[i];
		ne=H.ne;
	}
	Hf=H.Hf;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
bool U_LT_Hash<TD,TK,LL>::init(size_t Size, size_t (*hf)(TK, size_t))
{
	if(Size>0)
	{
		size=Size;
		if(P)delete[] P;
		try{ P=new LL[size]; }
		catch(...) { P=NULL; }
		if(!P)
		{
			OUT_ERROR("no enough memory for Hash; size=%d\n", size);
			ne=size=0;
			return false;
//			throw ("bool U_LT_Hash::init(int Size, int (*)(TK, int)) : no enough memory\n");
		}
	}
	else if(size) return false;
	Hf=hf;
	ne=0;
	return true;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
int U_LT_Hash<TD,TK,LL>::insert(TK key,const TD& data,unsigned int FR_flag)
{
	LL *ll;
	int i;
#ifndef UHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LT_Hash::insert(const TK, const TD&, unsigned int) : using uninitialized struct\n");
		return UHASH_NOTINIT;
	}
#endif
	i=Hf(key,size)%size;
	ll=P+abs(i);
	if(ll->insert(key,data,FR_flag)>=0)ne++;
	return ne;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
int U_LT_Hash<TD,TK,LL>::insert(const TK& key,const TD* data,unsigned int FR_flag)
{
	LL *ll;
	int i;
#ifndef UHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LT_Hash::insert(const TK&, const TD*, unsigned int) : using uninitialized struct\n");
		return UHASH_NOTINIT;
	}
#endif
	i=Hf(key,size)%size;
	ll=P+abs(i);
	if(ll->insert(key,data,FR_flag)>=0)ne++;
	return ne;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
TD* U_LT_Hash<TD,TK,LL>::insert_adress(const TK& key,const TD& data, unsigned int FR_flag)
{
	LL *ll;
	int i;
	TD *res;
#ifndef UHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LT_Hash::insert_adress(const TK&, const TD&, unsigned int) : using uninitialized struct\n");
		return NULL;
	}
#endif
	i=Hf(key,size)%size;
	ll=P+abs(i);
	i=ll->num_elements();
	res = ll->insert_adress(key, data, FR_flag);
	ne+=ll->num_elements()-i;
	return res;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
TD* U_LT_Hash<TD,TK,LL>::insert_adress(const TK& key, unsigned int FR_flag)
{
	LL *ll;
	int i;
	TD *res;
#ifndef UHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LT_Hash::insert_adress(const TK&, unsigned int) : using uninitialized struct\n");
		return NULL;
	}
#endif
	i=Hf(key,size)%size;
	ll=P+abs(i);
	i=ll->num_elements();
	res = ll->insert_adress(key, FR_flag);
	ne+=ll->num_elements()-i;
	return res;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
int U_LT_Hash<TD,TK,LL>::delete_(TK key, int N)
{
	LL *ll;
	size_t i;
#ifndef UHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LT_Hash::delete_(const TK&, int) : using uninitialized struct\n");
		return UHASH_NOTINIT;
	}
#endif
	i=Hf(key,size)%size;
	ll=P+abs(i);
	if((i=ll->_delete(key,N))>=0)
	{
		if(N==UHASH_DELETEALL)ne-=i;
		else ne--;
	}
	return ne;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
inline int U_LT_Hash<TD,TK,LL>::_delete(const TK& key, int N)
{
	return delete_(key,N);
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
void U_LT_Hash<TD,TK,LL>::clear()
{
	size_t i;
	if((!P))return;
	for(i=0; i<size; i++)
		P[i].clear();
	ne=0;
}

//--------------------------------------------
template <typename TD, typename TK, class LL>
U_LT_Hash<TD,TK,LL>::~U_LT_Hash()
{
	delete[] P;
#ifndef UHASH_RELEASE
	P=NULL;
	size=0;
	ne=0;
#endif
}

//--------------------------------------------
template <typename TD, typename TK, class LL>
TD* U_LT_Hash<TD,TK,LL>::operator [](TK key)
{
	size_t i;
#ifndef UHASH_RELEASE
	if((!P)||(!Hf))
	{
		OUT_ERROR("using uninitialized hash\n");
		return NULL;
	}
#endif
	i=((size_t)Hf(key, size))%size;
	return P[i][key];
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
U_LT_Hash<TD,TK,LL>& U_LT_Hash<TD,TK,LL>::operator=(const U_LT_Hash<TD,TK,LL>& H)
{
	size_t i;
	if(&H==this)return *this;
	Hf=H.Hf;
	if(size!=H.size)
	{
		delete[]P;
		size=H.size;
		if(size>=0)
		{
			try{ P=new LL[size]; }
			catch(...) { P=NULL; }
			if(!P)
			{
				OUT_ERROR("no enough memory for hash : size = %d (%d bytes)\n", size, size*sizeof(LL));
				size=0;
				throw ss_alloc_except("U_LT_Hash::operator=(const U_LT_Hash &)", H.size*sizeof(LL));
			}
		}
		else 
		{
			P=NULL;
			size=0;
		}
	}
	else for(i=0; i<size; i++)P[i].clear();
	for(i=0; i<size; i++)P[i]=H.P[i];
	ne=H.ne;
	return *this;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
int U_LT_Hash<TD,TK,LL>::print(FILE* fp, int (*Print)(FILE*, const TK&, const TD&))const
{
	int res=0;
	size_t i;
	if(!P||!Hf)return fprintf(fp, "<not initialized>");
	for(i=0; i<size; i++)
	{
		if(!P[i].num_elements())continue;
		res+=fprintf(fp, "[%d]={", i);
		res+=P[i].print(fp, Print);
		res+=fprintf(fp, "}\n");
	}
	return res;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
TD* U_LT_Hash<TD,TK,LL>::get(const TK& key)
{
	size_t i;
#ifndef UHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LT_Hash::get(const TK, const TD&, unsigned int) : using uninitialized struct\n");
		return NULL;
	}
#endif
	i=Hf(key,size)%size;
	return P[i].get(key);
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
const TK* U_LT_Hash<TD,TK,LL>::get_key(const TK& key) const
{
	size_t i;
#ifndef UHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LT_Hash::insert(const TK, const TD&, unsigned int) : using uninitialized struct\n");
		return NULL;
	}
#endif
	i=Hf(key,size)%size;
	return P[i].get_key(key);
}
///============================//
///=======CLASS I_Hash==========//
template <typename TD, typename TK>
int (* I_Hash<TD,TK>::DefHf)(const TK&,int,int,int)=NULL;
//--------------------------------------------
/*template <typename TD, typename TK>
bool I_Hash<TD,TK>::SetDefHf(int (*def_hf)(TK&,int,int,int))
{
	if((DefHf)&&(!def_hf))return false;
	DefHf=def_hf;
	return true;
}
//--------------------------------------------
template <typename TD, typename TK>
bool I_Hash<TD,TK>::SetHf(int (*def_hf1)(TK&,int,int,int),int (*def_hf2)(TK&,int,int,int))
{
	if(((h1)&&(P))||(!def_hf))return false;
	if(def_hf1)
	{
		h1=def_hf1;
		if(def_hf2)h2=def_hf2;
		else h2=h1;
	}
	else if(def_hf2)h1=h2=def_hf2;
	else return false;
	return true;
}
//--------------------------------------------
template <typename TD, typename TK>
I_Hash<TD,TK>::I_Hash()
{
	hf_par1=17;
	hf_par2=101;
	h2=h1=DefHf;
	P=NULL;
	ne=0;
}
//--------------------------------------------
template <typename TD, typename TK>
I_Hash<TD,TK>::I_Hash(int Size,TK *keys,TD *values,int (*hf1)(TK &,int,int,int),int (*hf2)(TK&,int,int,int))
{
	ne=0;
	P=NULL;
	if(hf1)
	{
		h1=hf1;
		if(hf2)h2=hf2;
		else h2=h1;
	}
	else if(hf2)h1=h2=hf2;
	else if(!(h1=h2=DefHf)||(Size<=0)||(!keys)||(!values))return;

	P=new IH_F[Size];
	int mem_size=5*Size, i,j;

	ne=Size;
	srand(clock());
	j=0;
	while((mem_size>(Size<<1))&&(j<IHASH_MAX_C)
	{
		hf_par1=rand()+rand()<<16;
		hf_par2=rand()+rand()<<16;
		for(i=0;i<size;i++)P[i].ne=0;
		for(i=0; i<Size; i++)
			P[h1(keys[i],Size,hf_par1,hf_par2)].ne++;
		for(i=0; i<Size; i++)
			mem_size+=P[i].ne*P[i].ne;
		j++;
	}
	P[0].D=new TD[mem_size];
	for(i=j=0; i<Size; i++)
	{

	}
}
//--------------------------------------------
template <typename TD, typename TK>
I_Hash<TD,TK>::I_Hash(int Size, int (*h1)(TK&,int))
{
	if(Size>0)size=Size;
	else size=(DefSize>0 ? DefSize : 0);
	if(size>0)	P=new LL[size];
	else P=NULL;
	h1=h1;
	ne=0;
}
//--------------------------------------------
template <typename TD, typename TK>
I_Hash<TD,TK>::I_Hash(I_Hash<TD,TK>& H)
{
	size=H.size;
	if(size>0)	P=new LL[size];
	if((P)&&(H.P))
	{
		for(int i=0; i<size; i++)
			P[i]=H[i];
		ne=H.ne;
	}
	else P=NULL;
	h1=H.h1;
}
//--------------------------------------------
template <typename TD, typename TK>
bool I_Hash<TD,TK>::init(int Size, int (*h1)(TK&,int))
{
	if(Size>0)
	{
		size=Size;
		if(P)delete[] P;
		P=new LL[size];
	}
	else if(size) return false;
	h1=h1;
	ne=0;
	return true;
}
//--------------------------------------------
template <typename TD, typename TK>
int I_Hash<TD,TK>::insert(TK& key,TD& data,unsigned int FR_flag)
{
	LL *ll;
	int i;
	if((!P)||(!h1))return UHASH_NOTINIT;
	i=h1(key,size)%size;
	ll=P+abs(i);
	if(ll->insert(key,data,FR_flag)>=0)ne++;
	return ne;
}
//--------------------------------------------
template <typename TD, typename TK>
int I_Hash<TD,TK>::insert(TK&& key,TD* data,unsigned int FR_flag)
{
	LL *ll;
	int i;
	if((!P)||(!h1))return UHASH_NOTINIT;
	i=h1(key,size)%size;
	ll=P+abs(i);
	if(ll->insert(key,data,FR_flag)>=0)ne++;
	return ne;
}
//--------------------------------------------
template <typename TD, typename TK>
int I_Hash<TD,TK>::delete_(TK& key, int N)
{
	LL *ll;
	int i;
	if((!P)||(!h1))return UHASH_NOTINIT;
	i=h1(key,size)%size;
	ll=P+abs(i);
	if((i=ll->_delete(key,N))>=0)
	{
		if(N==UHASH_DELETEALL)ne-=i;
		else ne--;
	}
	return ne;
}
//--------------------------------------------
template <typename TD, typename TK>
inline int I_Hash<TD,TK>::_delete(TK&& key, int N)
{
	return delete_(key,N);
}
//--------------------------------------------
template <typename TD, typename TK>
void I_Hash<TD,TK>::clear()
{
	int i;
	if((!P))return;
	for(i=0; i<size; i++)
		P[i].clear();
	ne=0;
}

//--------------------------------------------
template <typename TD, typename TK>
I_Hash<TD,TK>::~I_Hash()
{
	delete[] P;
	P=NULL;
	size=0;
	ne=0;
}

//--------------------------------------------
template <typename TD, typename TK>
TD* I_Hash<TD,TK>::operator [](TK& key)
{
	int i;
	if((!P)||(!h1))return NULL;
	i=h1(key,size)%size;
	return P[i][key];
}
//--------------------------------------------
template <typename TD, typename TK>
TD* I_Hash<TD,TK>::get(TK&& key)
{
	int i;
	if((!P)||(!h1))return NULL;
	i=h1(key,size)%size;
	return P[i].get(key);
}*/
///////////////////////////////////////////////////////////
///===============================//
///======= CLASS U_LTKA_Hash ======//
//template <typename TD, typename TK,class LL>
//int U_LTKA_Hash<TD,TK,LL>::DefSize=U_HASH_DEFSIZE;
//--------------------------------------------
template <typename TD, typename TK, class LL>
size_t (* U_LTKA_Hash<TD,TK,LL>::DefHf)(const TK&,size_t)=NULL;
//--------------------------------------------
template <typename TD, typename TK, class LL>
bool U_LTKA_Hash<TD,TK,LL>::SetDefHf(size_t (*def_hf)(const TK&,size_t))
{
	if((DefHf)&&(!def_hf))return false;
	DefHf=def_hf;
	return true;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
bool U_LTKA_Hash<TD,TK,LL>::SetHf(size_t (*def_hf)(const TK&, size_t))
{
	if(((Hf)&&(ne))||(!def_hf))return false;
	Hf=def_hf;
	return true;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
U_LTKA_Hash<TD,TK,LL>::U_LTKA_Hash()
{
	if(DefSize>0) {P=new LL[DefSize]; size=DefSize;}
	else {P=NULL; size=0;}
	if(DefHf) Hf=DefHf;
	else Hf=NULL;
	ne=0;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
U_LTKA_Hash<TD,TK,LL>::U_LTKA_Hash(size_t Size)
{
	if(Size>0)size=Size;
	else size=(DefSize>0 ? DefSize : 0);
	if(size>0)
	{
		try{ P=new LL[size]; }
		catch(...) { P=NULL; }
		if(!P)
		{
			OUT_ERROR("no enough memory for Hash; size=%d\n", size);
			ne=size=0;
			throw ss_alloc_except("constructor U_LTKA_Hash(size_t)", Size*sizeof(LL));
		}
	}
	else P=NULL;
	Hf=DefHf;
	ne=0;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
U_LTKA_Hash<TD,TK,LL>::U_LTKA_Hash(size_t Size, size_t (*hf)(const TK&,size_t))
{
	if(Size>0)size=Size;
	else size=(DefSize>0 ? DefSize : 0);
	if(size>0)
	{
		try{ P=new LL[size]; }
		catch(...) { P=NULL; }
		if(!P)
		{
			OUT_ERROR("no enough memory for Hash; size=%d\n", size);
			ne=size=0;
			throw ss_alloc_except("constructor U_LTKA_Hash(size_t, size_t (*)(const TK &, size_t))", DefSize*sizeof(LL));
		}
	}
	else P=NULL;
	Hf=hf;
	ne=0;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
U_LTKA_Hash<TD,TK,LL>::U_LTKA_Hash(const U_LTKA_Hash<TD,TK,LL>& H)
{
	size=H.size;
	if(size>0)
	{
		try{ P=new LL[size]; }
		catch(...) { P=NULL; }
		if(!P)
		{
			OUT_ERROR("no enough memory for Hash; size=%d\n", size);
			ne=size=0;
			throw ss_alloc_except("constructor U_LTKA_Hash(const U_LTKA_Hash &)", H.size*sizeof(LL));
		}
	}
	if((P)&&(H.P))
	{
		for(size_t i=0; i<size; i++)
			P[i]=H.P[i];
		ne=H.ne;
	}
	else P=NULL;
	Hf=H.Hf;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
U_LTKA_Hash<TD,TK,LL>::~U_LTKA_Hash()
{
	delete[] P;
#ifndef UKAHASH_RELEASE
	P=NULL;
	size=0;
	ne=0;
#endif
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
bool U_LTKA_Hash<TD,TK,LL>::init(size_t Size, size_t (*hf)(const TK&,size_t))
{
	if(Size>0)
	{
		size=Size;
		if(P)delete[] P;
		try{ P=new LL[size]; }
		catch(...) { P=NULL; }
		if(!P)
		{
			OUT_ERROR("no enough memory for Hash; size=%d\n", size);
			ne=size=0;
			return false;
		}
	}
	else if(size) return false;
	Hf=hf;
	ne=0;
	return true;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
int U_LTKA_Hash<TD,TK,LL>::insert(const TK& key, const TD& data,unsigned int FR_flag)
{
	LL *ll;
	size_t i;
#ifndef UKAHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LTKA_Hash::insert(const TK&, const TD&, unsigned int) : using uninitialized struct\n");
		return UHASH_NOTINIT;
	}
#endif
	i=Hf(key,size)%size;
	ll=P+abs(i);
	if(ll->insert(key, &data, FR_flag)>=0)ne++;
	return ne;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
int U_LTKA_Hash<TD,TK,LL>::insert(const TK& key, const TD* data,unsigned int FR_flag)
{
	LL *ll;
	size_t i;
#ifndef UKAHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LTKA_Hash::insert(const TK&, const TD*, unsigned int) : using uninitialized struct\n");
		return UHASH_NOTINIT;
	}
#endif
	i=Hf(key,size)%size;
	ll=P+abs(i);
	if(ll->insert(key,data,FR_flag)>=0)ne++;
	return ne;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
TD* U_LTKA_Hash<TD,TK,LL>::insert_adress(const TK& key,const TD& data, unsigned int FR_flag)
{
	LL *ll;
	size_t i;
	TD *res;
#ifndef UKAHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LTKA_Hash::insert_adress(const TK&, const TD&, unsigned int) : using uninitialized struct\n");
		return NULL;
	}
#endif
	i=Hf(key,size)%size;
	ll=P+abs(i);
	i=ll->num_elements();
	res = ll->insert_adress(key, data, FR_flag);
	ne+=ll->num_elements()-i;
	return res;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
TD* U_LTKA_Hash<TD,TK,LL>::insert_adress(const TK& key, unsigned int FR_flag)
{
	LL *ll;
	size_t i;
	TD *res;
#ifndef UKAHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LTKA_Hash::insert_adress(const TK&, const TD&, unsigned int) : using uninitialized struct\n");
		return NULL;
	}
#endif
	i=Hf(key,size)%size;
	ll=P+abs(i);
	i=ll->num_elements();
	res = ll->insert_adress(key, FR_flag);
	ne+=ll->num_elements()-i;
	return res;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
int U_LTKA_Hash<TD,TK,LL>::delete_(const TK& key, int N)
{
	LL *ll;
	size_t i;
#ifndef UKAHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LTKA_Hash::delete_(const TK&, int) : using uninitialized struct\n");
		return UHASH_NOTINIT;
	}
#endif
//	if((!P)||(!Hf))return UHASH_NOTINIT;
	i=Hf(key,size)%size;
	ll=P+abs(i);
	if((i=ll->_delete(key,N))>=0)
	{
		if(N==UHASH_DELETEALL)ne-=i;
		else ne--;
	}
	return ne;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
int U_LTKA_Hash<TD,TK,LL>::_delete(const TK& key, int N)
{
	return delete_(key,N);
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
void U_LTKA_Hash<TD,TK,LL>::clear()
{
	size_t i;
	if((!P))return;
	for(i=0; i<size; i++)
		P[i].clear();
	ne=0;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
U_LTKA_Hash<TD,TK,LL>& U_LTKA_Hash<TD,TK,LL>::operator=(const U_LTKA_Hash<TD,TK,LL>& H)
{
	size_t i;
	if(&H==this)return *this;
/*#ifndef UKAHASH_RELEASE
	if(!(Hf=H.Hf))
	{
		if(error)error("U_LTKA_Hash::insert_adress(const TK&, const TD&, unsigned int) : using uninitialized struct\n");
		return *this;
	}
#else*/
	Hf=H.Hf;
//#endif
	if(size!=H.size)
	{
		delete[]P;
		size=H.size;
		if(size>0)
		{
			try{ P=new LL[size]; }
			catch(...) { P=NULL; }
			if(!P)
			{
				OUT_ERROR("no enough memory for Hash; size=%d\n", size);
				ne=size=0;
				throw ss_alloc_except("U_LTKA_Hash::operator=(const U_LTKA_Hash &)", H.size*sizeof(LL));
			}
		}
		else 
		{
			P=NULL;
			size=0;
		}
	}
	else for(i=0; i<size; i++)P[i].clear();
	for(i=0; i<size; i++)P[i]=H.P[i];
	ne=H.ne;
	return *this;
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
TD* U_LTKA_Hash<TD,TK,LL>::get(const TK& key)
{
	size_t i;
#ifndef UKAHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LTKA_Hash::delete_(const TK&, int) : using uninitialized struct\n");
		return NULL;
	}
#endif
	i=Hf(key,size)%size;
	return P[i].get(key);
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
const TK* U_LTKA_Hash<TD,TK,LL>::get_key(const TK& key) const
{
	size_t i;
#ifndef UKAHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LTKA_Hash::delete_(const TK&, int) : using uninitialized struct\n");
		return NULL;
	}
#endif
	i=Hf(key,size)%size;
	return P[i].get_key(key);
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
TD* U_LTKA_Hash<TD,TK,LL>::operator [](const TK& key)
{
	int i;
#ifndef UKAHASH_RELEASE
	if((!P)||(!Hf))
	{
		if(error)error("U_LTKA_Hash::operator [](const TK&) : using uninitialized struct\n");
		return NULL;
	}
#endif
	i=Hf(key,size)%size;
	return P[i][key];
}
//--------------------------------------------
template <typename TD, typename TK, class LL>
int U_LTKA_Hash<TD,TK,LL>::print(FILE* fp, int (*Print)(FILE*, const TK&, const TD&))const
{
	int res=0;
	size_t i;
	if(!P||!Hf)return fprintf(fp, "<not initialized>");
	for(i=0; i<size; i++)
	{
		if(!P[i].num_elements())continue;
		res+=fprintf(fp, "[%d]={", i);
		res+=P[i].print(fp, Print);
		res+=fprintf(fp, "}\n");
	}
	return res;
}
///=============================//
///=======CLASS SLL_item==========//
	
template <typename TD, typename TK>
const SLL_item<TD,TK> *SLL_item<TD,TK>::nil=&Nil;//new SLL_item<TD,TK>();
//--------------------------------
template <typename TD, typename TK>
const SLL_item<TD,TK> SLL_item<TD,TK>::Nil;
//______________________________________________________
template <typename TD, typename TK>
SLL_item<TD,TK>::SLL_item()
{
	next=NULL;
}
//--------------------------------
template <typename TD, typename TK>
SLL_item<TD,TK>::SLL_item(TK Key, const TD &value)
{
	next=NULL;
	key=Key; data=value;//correct only if for type TK exists correct operator=
						//(TK cannot be a pointer)
}
//--------------------------------
template <typename TD, typename TK>
SLL_item<TD,TK>::SLL_item(TK Key, const TD *value, SLL_item<TD,TK>*)
{
	next=NULL;
	key=Key;	//correct only if for type TK exists correct operator=
				//(TK cannot be a pointer)
	if(value)data = *value;
}
//--------------------------------
template <typename TD, typename TK>
SLL_item<TD,TK>::SLL_item(const SLL_item<TD,TK> &LI)
{
	if((&LI)!=NULL)
	{
		key=LI.key;
		data=LI.data;
		if(LI.next!=NULL) next=new SLL_item<TD,TK>(*LI.next);
		else next=NULL;
	}
	else next=NULL; 
}
//--------------------------------
template <typename TD, typename TK>
SLL_item<TD,TK>& SLL_item<TD,TK>::operator=(const SLL_item<TD,TK> &LI)
{
	if(((&LI)!=NULL)&&((&LI)!=this))
	{
		key=LI.key;
		data=LI.data;
	}
	return *this;
}
//--------------------------------
template <typename TD, typename TK>
SLL_item<TD,TK>::~SLL_item()
{
	if(!(next))return;
	SLL_item<TD,TK> *Curr=this,*Next=next;
	while(Next)
	{
		Curr=Next;
		Next=Next->next;
		Curr->next=NULL;
		delete Curr;
	}
	next=NULL;
}

///=================================//
///======= CLASS SLL_KA_item==========//
//__ item of linked list with keys which are structs or classes__
	
template <typename TD, typename TK>
const SLL_KA_item<TD,TK> *SLL_KA_item<TD,TK>::nil=&Nil;//new SLL_KA_item<TD,TK>();
//______________________________________________________
template <typename TD, typename TK>
const SLL_KA_item<TD,TK> SLL_KA_item<TD,TK>::Nil;
//______________________________________________________
template <typename TD, typename TK>
SLL_KA_item<TD,TK>::SLL_KA_item()
{
	next=NULL;
}
//--------------------------------
template <typename TD, typename TK>
SLL_KA_item<TD,TK>::SLL_KA_item(const TK& Key, const TD &value)
{
	next=NULL;
	key=Key; data=value;//correct only if for type TK exists correct operator=
						//(TK& cannot be a pointer)
}
//--------------------------------
template <typename TD, typename TK>
SLL_KA_item<TD,TK>::SLL_KA_item(const TK& Key, const TD *value, SLL_KA_item<TD,TK> *)
{
	next=NULL;
	key=Key;
	//correct only if for type TK exists correct operator=
	//(TK& cannot be a pointer)
	if(value)data=*value;
}
//--------------------------------
template <typename TD, typename TK>
SLL_KA_item<TD,TK>::SLL_KA_item(const SLL_KA_item<TD,TK> &LI)
{
	if((&LI)!=NULL)
	{
		key=LI.key;
		data=LI.data;
		if(LI.next!=NULL)
		{
			try{ next=new SLL_KA_item<TD,TK>(*LI.next); }
			catch(bad_alloc BA)	{ next=NULL; }
			if(!next)
			{
				OUT_ERROR(" no enough memory for linked list : size = %d\n", sizeof(*next));
				throw ss_alloc_except("constructor SLL_KA_item(const SLL_KA_item &)", sizeof(*next));
			}
		}
		else next=NULL;
	}
	else next=NULL; 
}
//--------------------------------
template <typename TD, typename TK>
SLL_KA_item<TD,TK>& SLL_KA_item<TD,TK>::operator=(const SLL_KA_item<TD,TK>& LI)
{
	if(((&LI)!=NULL)&&((&LI)!=this))
	{
		key=LI.key;
		data=LI.data;
	}
	return *this;
}
//--------------------------------
template <typename TD, typename TK>
SLL_KA_item<TD,TK>::~SLL_KA_item()
{
	if(!(next))return;
	SLL_KA_item<TD,TK> *Curr=this,*Next=next;
	while(Next)
	{
		Curr=Next;
		Next=Next->next;
		Curr->next=NULL;
		delete Curr;
	}
	next=NULL;
}
///=============================//
///=======CLASS LL_item==========//
	
template <typename TD, typename TK>
const LL_item<TD,TK> *LL_item<TD,TK>::nil=&Nil;//new LL_item<TD,TK>();
//______________________________________________________
template <typename TD, typename TK>
const LL_item<TD,TK> LL_item<TD,TK>::Nil;
//______________________________________________________
template <typename TD, typename TK>
LL_item<TD,TK>::LL_item()
{
	next=prev=NULL;
}
//--------------------------------
template <typename TD, typename TK>
LL_item<TD,TK>::LL_item(TK Key, const TD &value)
{
	next=prev=NULL;
	key=Key; data=value;//correct only if for type TK exists correct operator=
						//(TK cannot be a pointer)
}
//--------------------------------
template <typename TD, typename TK>
LL_item<TD,TK>::LL_item(TK Key, const TD *value, LL_item<TD,TK> * Prev)
{
	next=NULL;
	prev=Prev;
	key=Key;	//correct only if for type TK exists correct operator=
				//(TK cannot be a pointer)
	if(value)data = *value;
}
//--------------------------------
template <typename TD, typename TK>
LL_item<TD,TK>::LL_item(const LL_item<TD,TK> &LI)
{
	if((&LI)!=NULL)
	{
		key=LI.key;
		data=LI.data;
		if(LI.next!=NULL)
		{
			try{ next=new LL_item<TD,TK>(*LI.next); }
			catch(bad_alloc BA)	{ next=NULL; }
			if(!next)
			{
//				OUT_ERROR(" no enough memory for linked list : size = %d\n", sizeof(*next));
				throw ss_alloc_except("constructor LL_item(const LL_item &)", sizeof(*next));
			}
			next->prev=this;
		}
		else next=NULL;
	}
	else next=NULL; 
	prev=NULL;
}
//--------------------------------
template <typename TD, typename TK>
LL_item<TD,TK>& LL_item<TD,TK>::operator=(const LL_item<TD,TK> &LI)
{
	if(((&LI)!=NULL)&&((&LI)!=this))
	{
		key=LI.key;
		data=LI.data;
	}
	return *this;
}
//--------------------------------
template <typename TD, typename TK>
LL_item<TD,TK>::~LL_item()
{
	prev=NULL;
	if(!(next))return;
	LL_item<TD,TK> *Curr=this,*Next=next;
	while(Next)
	{
		Curr=Next;
		Next=Next->next;
		Curr->next=NULL;
		delete Curr;
	}
	next=NULL;
}

///=================================//
///======= CLASS LL_KA_item==========//
//__ item of linked list with keys which are structs or classes__
	
template <typename TD, typename TK>
const LL_KA_item<TD,TK> *LL_KA_item<TD,TK>::nil=&Nil;//new LL_KA_item<TD,TK>();
//______________________________________________________
template <typename TD, typename TK>
const LL_KA_item<TD,TK> LL_KA_item<TD,TK>::Nil;
//______________________________________________________
template <typename TD, typename TK>
LL_KA_item<TD,TK>::LL_KA_item()
{
	next=prev=NULL;
}
//--------------------------------
template <typename TD, typename TK>
LL_KA_item<TD,TK>::LL_KA_item(const TK& Key, const TD &value)
{
	next=prev=NULL;
	key=Key; data=value;//correct only if for type TK exists correct operator=
						//(TK& cannot be a pointer)
}
//--------------------------------
template <typename TD, typename TK>
LL_KA_item<TD,TK>::LL_KA_item(const TK& Key, const TD *value, LL_KA_item<TD, TK> * Prev)
{
	next=NULL;
	prev=Prev;
	key=Key;
	//correct only if for type TK exists correct operator=
	//(unsafe if TK is a pointer)
	if(value)data=*value;
}
//--------------------------------
template <typename TD, typename TK>
LL_KA_item<TD,TK>::LL_KA_item(const LL_KA_item<TD,TK> &LI)
{
	if((&LI)!=NULL)
	{
		key=LI.key;
		data=LI.data;
		if(LI.next!=NULL)
		{
			try{ next=new LL_KA_item<TD,TK>(*LI.next); }
			catch(bad_alloc BA)	{ next=NULL; }
			if(!next)
			{
//				OUT_ERROR(" no enough memory for linked list : size = %d\n", sizeof(*next));
				throw ss_alloc_except("constructor LL_KA_item(const LL_KA_item &)", sizeof(*next));
			}
			next->prev=this;
		}
		else next=NULL;
	}
	else next=NULL; 
	prev=NULL;
}
//--------------------------------
template <typename TD, typename TK>
LL_KA_item<TD,TK>& LL_KA_item<TD,TK>::operator=(const LL_KA_item<TD,TK>& LI)
{
	if(((&LI)!=NULL)&&((&LI)!=this))
	{
		key=LI.key;
		data=LI.data;
	}
	return *this;
}
//--------------------------------
template <typename TD, typename TK>
LL_KA_item<TD,TK>::~LL_KA_item()
{
	prev=NULL;
	if(!(next))return;
	LL_KA_item<TD,TK> *Curr=this,*Next=next;
	while(Next)
	{
		Curr=Next;
		Next=Next->next;
		Curr->next=NULL;
		delete Curr;
	}
	next=NULL;
}
//________________________________________________
///============================//
///=======CLASS L_List_LT =======//

template<typename TD, typename TK, class LLI>
L_List_LT<TD,TK,LLI>::L_List_LT()
{
	L=last=NULL;
	ne=0;
	return;
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
L_List_LT<TD,TK,LLI>::L_List_LT(const L_List_LT<TD,TK,LLI> &LL)
{
	ne=0;
	if(LL.L)
	{
		LLI ** t_to = &L, * t_from, * prev=NULL;
		for(t_from=LL.L; t_from; t_from=t_from->next, ne++)
		{
			try{ prev=new LLI(t_from->key, &(t_from->data), prev); }
			catch(bad_alloc BA)	{ prev=NULL; }
			if(!prev)
			{
				OUT_ERROR(" no enough memory for linked list : size = %d\n", (LL.ne-ne)*sizeof(LLI));
				throw ss_alloc_except("constructor L_List_LT(const L_List_LT &)", sizeof(LLI));
			}
			*t_to=prev;
			t_to=&(prev->next);
		}
		last=prev;
	}
	else last=L=NULL;
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
L_List_LT<TD,TK,LLI>& L_List_LT<TD,TK,LLI>::operator=(const L_List_LT<TD,TK,LLI> &LL)
{
	ne=0;
	if((&LL)==this)return *this;
	if(LL.L==NULL)
	{
		if(L!=NULL)delete L;
		L=NULL;
	}
	else
	{
		LLI ** t_to = &L, * t_from, * prev=NULL, * curr;
		if(L)
		{
			for(t_from=LL.L, curr=L; t_from && curr; t_from=t_from->next, ne++)
			{
				*curr=*t_from;
				prev=curr;
				curr=curr->next;
			}
			if(t_from)t_to=&(prev->next);
			else
			{
				last=prev;
				delete curr;
				return *this;
			}
		}
		for(; t_from; t_from=t_from->next, ne++)
		{
			try{ prev=new LLI(t_from->key, &(t_from->data), prev); }
			catch(bad_alloc BA)	{ prev=NULL; }
			if(!prev)
			{
				OUT_ERROR(" no enough memory for linked list : size = %d\n", (LL.ne-ne)*sizeof(LLI));
				throw ss_alloc_except("L_List_LT::operator=(const L_List_LT &)", sizeof(LLI));
			}
			*t_to=prev;
			t_to=&(prev->next);
		}
		last=prev;
	}
	return *this;
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
TD* L_List_LT<TD,TK,LLI>::get(const TK& Key)
{
	LLI *t=L;
	while((t!=NULL)&&(t->key != Key))	t=t -> next;
	if(t) return &(t->data);
	return NULL;
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
const TK* L_List_LT<TD,TK,LLI>::get_key(const TK& Key) const
{
	LLI *t=L;
	while((t!=NULL)&&(t->key != Key))	t=t -> next;
	if(t) return &(t->key);
	return NULL;
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
int L_List_LT<TD,TK,LLI>::get(const TK *Key, TD &res, int N)const 
{
	int i,j;
	LLI *t=L;
	for(i=j=0;(t);i++)
	{
		if(t->key == *Key) j++;
		if(j>N) break;
		t = t->next;
	}
	if(t)
	{
		res = t->data;
		return i;
	}
	return -1;
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
L_List_LT<TD,TK,LLI>::~L_List_LT()
{
	if(L!=NULL)
	{
		delete L;
		L = NULL;
	}
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
void L_List_LT<TD,TK,LLI>::clear()
{
	if(L!=NULL)
	{
		delete L;
		L = NULL;
	}
}

//------------------------------------------------
template<typename TD, typename TK, class LLI>
int L_List_LT<TD,TK,LLI>::print(FILE *fp, int (*PrintKD)(FILE*, const TK&, const TD&)) const 
{
	LLI *LI;
	if(L)LI=L->next;
	else return fprintf(fp, "<empty>");
	int res=0;
	res+=PrintKD(fp, L->key, L->data);
	for(res=0;(LI);LI=LI->next)
	{
		res+=fprintf(fp, ", ");
		res+=PrintKD(fp, LI->key, LI->data);
	}
	return res;
}


///============================//
///=======CLASS SL_List_LT==========//

template<typename TD, typename TK, class LLI>
SL_List_LT<TD,TK,LLI>& SL_List_LT<TD,TK,LLI>::operator=(const SL_List_LT<TD,TK,LLI> &LL)
{
	L_List_LT<TD,TK,LLI>::operator =(LL);
	return *this;
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
int SL_List_LT<TD,TK,LLI>::insert(const TK *Key, const TD& value, unsigned int FR_flag)
{
	int i;
	LLI *LI=NULL,*L1, *Prev=NULL;
	if(!key)return -1;
	if(FR_flag&UHASH_FRF_FIND)
		for(i=0, LI=L; (LI)&&(LI->key != *Key); LI = LI->next, i++) Prev=LI;
	if(LI==NULL)
	{
		try{ LI = new LLI(*Key, value); }
		catch(bad_alloc BA)	{ LI=NULL; }
		if(!LI)
		{
			OUT_ERROR(" no enough memory for linked list : SL_List_LT::insert(const TK *, const TD &, unsigned int) : size = %d\n", sizeof(LLI));
			throw ss_alloc_except("SL_List_LT::insert(const TK *, const TD &, unsigned int)", sizeof(LLI));
		}
		LI->next = L;
		L=LI;
		ne++;
		return 0;
	}
	switch(FR_flag&FRF_DOIFEXIST)
	{
	case FRF_REPLACE:
		LI->data = value;
		return i;
	case FRF_ADDAFTER:
		try{ L1 = new LLI(*Key, value); }
		catch(bad_alloc BA)	{ L1=NULL; }
		if(!L1)
		{
			OUT_ERROR(" no enough memory for linked list : SL_List_LT::insert(const TK *, const TD &, unsigned int) : size = %d\n", sizeof(LLI));
			throw ss_alloc_except("SL_List_LT::insert(const TK *, const TD &, unsigned int)", sizeof(LLI));
		}
		L1->next = LI->next;
		LI->next=L1;
		ne++;
		return i+1;
	case FRF_ADDBEFORE:
		try{ L1 = new LLI(*Key, value); }
		catch(bad_alloc BA)	{ L1=NULL; }
		if(!L1)
		{
			OUT_ERROR(" no enough memory for linked list : SL_List_LT::insert(const TK *, const TD &, unsigned int) : size = %d\n", sizeof(LLI));
			throw ss_alloc_except("SL_List_LT::insert(const TK *, const TD &, unsigned int)", sizeof(LLI));
		}
		if(Prev) Prev->next = L1;
		else L=L1;
		ne++;
		L1->next=LI;
		return i;
	}
	return -1;
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
int SL_List_LT<TD,TK,LLI>::insert(const TK& Key, const TD* value, unsigned int FR_flag)
{
	return insert(&Key, *value, FR_flag);
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
TD* SL_List_LT<TD,TK,LLI>::insert_adress(const TK &Key, const TD &value, unsigned int FR_flag)
{
	int i;
	LLI *LI=NULL,*L1, *Prev=NULL;
	TD *res;
	if(FR_flag&UHASH_FRF_FIND)
		for(i=0, LI=L; (LI)&&(LI->key != Key); LI = LI->next,i++) Prev=LI;
	if(LI==NULL)
	{
		try{ LI = new LLI(Key, value); }
		catch(bad_alloc BA)	{ LI=NULL; }
		if(!LI)
		{
			OUT_ERROR(" no enough memory for linked list : SL_List_LT::insert_adress(const TK &, const TD &, unsigned int) : size = %d\n", sizeof(LLI));
			throw ss_alloc_except("SL_List_LT::insert_adress(const TK &, const TD &, unsigned int)", sizeof(LLI));
		}
		res=&(LI->data);
		LI->next = L;
		ne++;
		L=LI;
		return res;
	}
	switch(FR_flag&FRF_DOIFEXIST)
	{
	case FRF_REPLACE:
		LI->data = value;
		return &(LI->data);
	case FRF_ADDAFTER:
		try{ L1 = new LLI(Key, value); }
		catch(bad_alloc BA)	{ L1=NULL; }
		if(!L1)
		{
			OUT_ERROR(" no enough memory for linked list : SL_List_LT::insert_adress(const TK &, const TD &, unsigned int) : size = %d\n", sizeof(LLI));
			throw ss_alloc_except("SL_List_LT::insert_adress(const TK &, const TD &, unsigned int)", sizeof(LLI));
		}
		res=&(L1->data);
		L1->next = LI->next;
		LI->next=L1;
		ne++;
		return res;
	case FRF_ADDBEFORE:
		try{ L1 = new LLI(Key, value); }
		catch(bad_alloc BA)	{ L1=NULL; }
		if(!L1)
		{
			OUT_ERROR(" no enough memory for linked list : SL_List_LT::insert_adress(const TK &, const TD &, unsigned int) : size = %d\n", sizeof(LLI));
			throw ss_alloc_except("SL_List_LT::insert_adress(const TK &, const TD &, unsigned int)", sizeof(LLI));
		}
		res=&(L1->data);
		if(Prev) Prev->next = L1;
		else L=L1;
		L1->next=LI;
		ne++;
		return res;
	}
	return &(LI->data);
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
TD* SL_List_LT<TD,TK,LLI>::insert_adress(const TK &Key, unsigned int FR_flag)
{
	int i;
	LLI *LI=NULL,*L1, *Prev=NULL;
	TD *res;
	if(FR_flag&UHASH_FRF_FIND)
		for(i=0, LI=L; (LI)&&(LI->key != Key); LI = LI->next, i++) Prev=LI;
	if(LI==NULL)
	{
		try{ LI = new LLI(Key, (TD*)NULL); }
		catch(bad_alloc BA)	{ LI=NULL; }
		if(!LI)
		{
			OUT_ERROR(" no enough memory for linked list : SL_List_LT::insert_adress(const TK &, unsigned int) : size = %d\n", sizeof(LLI));
			throw ss_alloc_except("SL_List_LT::insert_adress(const TK &, unsigned int)", sizeof(LLI));
		}
		res=&(LI->data);
		LI->next = L;
		ne++;
		L=LI;
		return res;
	}
	switch(FR_flag&FRF_DOIFEXIST)
	{
	case FRF_REPLACE:
		LI->data = value;
		return &(LI->data);
	case FRF_ADDAFTER:
		try{ L1 = new LLI(Key, (TD*)NULL); }
		catch(bad_alloc BA)	{ L1=NULL; }
		if(!L1)
		{
			OUT_ERROR(" no enough memory for linked list : SL_List_LT::insert_adress(const TK &, unsigned int) : size = %d\n", sizeof(LLI));
			throw ss_alloc_except("SL_List_LT::insert_adress(const TK &, unsigned int)", sizeof(LLI));
		}
		res=&(L1->data);
		L1->next = LI->next;
		LI->next=L1;
		ne++;
		return res;
	case FRF_ADDBEFORE:
		try{ L1 = new LLI(Key, (TD*)NULL); }
		catch(bad_alloc BA)	{ L1=NULL; }
		if(!L1)
		{
			OUT_ERROR(" no enough memory for linked list : SL_List_LT::insert_adress(const TK &, unsigned int) : size = %d\n", sizeof(LLI));
			throw ss_alloc_except("SL_List_LT::insert_adress(const TK &, unsigned int)", sizeof(LLI));
		}
		res=&(L1->data);
		if(Prev) Prev->next = L1;
		else L=L1;
		L1->next=LI;
		ne++;
		return res;
	}
	return &(LI->data);
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
int SL_List_LT<TD,TK,LLI>::delete_(TK *Key, int N)
{
	int i,j,k=-1;
	LLI *t=NULL,*s=L, *Prev;
	j=(N==LL_DELETEALL ? N : 0);
	i=0;
	do
	{
		Prev=t;
		t=s;
		s=t->next;
		if(t->key == *Key)
		{
			if(N==LL_DELETEALL)
			{
				t->next=NULL;
				if(Prev)Prev->next=s;
				else L=s;
				delete t;
				k++;
			}
			else j++;
		}
		i++;
	}
	while((t!=NULL)&&(j<=N));
	if(t)
	{
		s=t->next;
		t->next=NULL;
		if(Prev)Prev->next=s;
		else L=s;
		delete t;
		return i;
	}
	if(k>=0)k++;
	return k;
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
int SL_List_LT<TD,TK,LLI>::_delete(const TK& Key, int N)
{
	return delete_(Key,N);
}

///============================//
///=======CLASS DL_List_LT==========//

//------------------------------------------------
template<typename TD, typename TK, class LLI>
DL_List_LT<TD,TK,LLI>& DL_List_LT<TD,TK,LLI>::operator=(const DL_List_LT<TD,TK,LLI> &LL)
{
	L_List_LT<TD,TK,LLI>::operator =(LL);
	return *this;
}
//------------------------------------------------
template <typename TD, typename TK, class LLI>
int DL_List_LT<TD,TK,LLI>::insert(const TK *Key, const TD &value, unsigned int FR_flag)
{
	int i;
	if(!Key)return -1;
	LLI *LI=NULL,*L1;
	if(FR_flag&UHASH_FRF_FIND)
		for(i=0,LI=L; (LI)&&(LI->key != *Key); LI = LI->next,i++);
	if(LI==NULL)
	{
		LI = new LLI(*Key, value);
		if(LI->next = L)
			L->prev = LI;
		L=LI;
		ne++;
		return 0;
	}
	switch(FR_flag&FRF_DOIFEXIST)
	{
	case FRF_REPLACE:
		LI->data = value;
		return i;
	case FRF_ADDAFTER:
		L1 = new LLI(*Key, value);
		if(L1->next = LI->next)
			LI->next->prev = L1;
		L1->prev=LI;
		LI->next=L1;
		ne++;
		return i+1;
	case FRF_ADDBEFORE:
		L1 = new LLI(*Key, value);
		if(L1->prev = LI->prev)
			LI->prev->next = L1;
		else L=L1;
		L1->next=LI;
		LI->prev=L1;
		ne++;
		return i;
	}
	return -1;
}
//------------------------------------------------
template <typename TD, typename TK, class LLI>
TD * DL_List_LT<TD,TK,LLI>::insert_adress(const TK &Key, const TD &value, unsigned int FR_flag)
{
	TD * d=insert_adress(Key, FR_flag);
	if(d)*d=value;
	return d;
}
//------------------------------------------------
template <typename TD, typename TK, class LLI>
TD * DL_List_LT<TD,TK,LLI>::insert_adress(const TK &Key, unsigned int FR_flag)
{
	int i;
	LLI *LI=NULL,*L1;
	TD *res;
	if(FR_flag&FRF_FIND)
		for(i=0,LI=L; (LI)&&(LI->key != Key); LI = LI->next,i++);
	if(LI==NULL)
	{
		LI = new LLI(Key, (TD*)NULL);
		res=&(LI->data);
		if(LI->next = L)
			L->prev = LI;
		L=LI;
		ne++;
		return res;
	}
	switch(FR_flag&FRF_DOIFEXIST)
	{
	case FRF_REPLACE:
//		LI->data = value;
		return &(LI->data);
	case FRF_ADDAFTER:
		L1 = new LLI(Key, (TD*)NULL);
		res=&(L1->data);
		if(L1->next = LI->next)
			LI->next->prev = L1;
		L1->prev=LI;
		LI->next=L1;
		ne++;
		return res;
	case FRF_ADDBEFORE:
		L1 = new LLI(Key, (TD*)NULL);
		res=&(L1->data);
		if(L1->prev = LI->prev)
			LI->prev->next = L1;
		else L=L1;
		ne++;
		L1->next=LI;
		LI->prev=L1;
		return res;
	}
	return &(LI->data);
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
int DL_List_LT<TD,TK,LLI>::insert(const TK& Key, const TD* value, unsigned int FR_flag)
{
	return insert(&Key, *value, FR_flag);
}
//------------------------------------------------
template <typename TD, typename TK, class LLI>
int DL_List_LT<TD,TK,LLI>::delete_(const TK *Key,int N)
{
	int i,j,k=-1;
	LLI *t=L,*s=L;
	j=(N==LL_DELETEALL ? N : 0);
	for(i=0;(s!=NULL)&&(j<=N);i++)
	{
		t=s;
		s=t->next;
		if(t->key == *Key)
		{
			if(N==LL_DELETEALL)
			{
				t->next=NULL;
				if(t->prev)t->prev->next=s;
				else L=s;
				if(s)s->prev=t->prev;
				delete t;
				k++;
			}
			else j++;
		}
	}
	if(t)
	{
		s=t->next;
		t->next=NULL;
		if(t->prev)t->prev->next=s;
		else L=s;
		if(s)s->prev=t->prev;
		delete t;
		return i;
	}
	if(k>=0)k++;
	return k;
}
//------------------------------------------------
template<typename TD, typename TK, class LLI>
int DL_List_LT<TD,TK,LLI>::_delete(const TK& Key,int N)
{
	return delete_(&Key, N);
}

///============================//
///=======CLASS L_List==========//

template <typename TD, typename TK>
L_List<TD,TK>& L_List<TD,TK>::operator=(const L_List<TD,TK> &LL)
{
	try{ DL_List_LT::operator =(LL); }
	catch(ss_alloc_except SAE)
	{
		SAE.Where="L_List::operator=(const L_List &)";
		throw(SAE);
	}
	return *this;
}
//------------------------------------------------
template <typename TD, typename TK>
int L_List<TD,TK>::insert(TK Key, const TD& value, unsigned int FR_flag)
{
	return DL_List_LT::insert(Key, &value, FR_flag);
}
//------------------------------------------------
template <typename TD, typename TK>
int L_List<TD,TK>::insert(const TK& Key, const TD* value, unsigned int FR_flag)
{
	return DL_List_LT::insert(Key, value, FR_flag);
}
//------------------------------------------------
template <typename TD, typename TK>
TD* L_List<TD,TK>::operator[](TK Key)
{
	return get(Key);
}
//------------------------------------------------
template <typename TD, typename TK>
TD* L_List<TD,TK>::get(const TK& Key)
{
	return L_List_LT::get(Key);
}
//------------------------------------------------
template <typename TD, typename TK>
int L_List<TD,TK>::get(TK Key, TD &res,int N)const 
{
	return get(&Key, res, N);
}
//------------------------------------------------
template <typename TD, typename TK>
int L_List<TD,TK>::delete_(TK Key,int N)
{
	return delete_(&Key, N);
}
///=============================//
///=======CLASS L_List_f==========//
template <typename TD, typename TK>
L_List_f<TD,TK>::L_List_f(const L_List_f<TD,TK> &LL)
{
	LL_item<TD,TK> *LI,**LI1=&L,*Prev=NULL;
	ne=LL.ne;
	for(LI=LL.L; LI; LI=LI->next,LI1=&((*LI1)->next))
	{
		*LI1=im.new_item();
		**LI1=*LI;
		(*LI1)->prev=Prev;
	}
	*LI1=NULL;
}
//------------------------------------------------
template <typename TD, typename TK>
L_List_f<TD,TK>& L_List_f<TD,TK>::operator=(const L_List_f<TD,TK> &LL)
{
	ne=LL.ne;
	if(LL.L==NULL)
	{
		if(L!=NULL)delete L;
		L=NULL;
	}
	else  if(L!=NULL) *L=*LL.L;
	else 
	{
		LL_item<TD,TK> *LI,**LI1=&L,*Prev=NULL;
		for(LI=LL.L; LI; LI=LI->next,LI1=&((*LI1)->next))
		{
			if(*LI1==NULL)*LI1 = im.new_item();
			**LI1 = *LI;
			(*LI1)->prev = Prev;
		}
		if(*LI1) im.delete_item(*LI1);
	}
	return *this;
}
//------------------------------------------------
template <typename TD, typename TK>
int L_List_f<TD,TK>::insert(TK Key, const TD& value, unsigned int FR_flag)
{
	int i;
	LL_item<TD,TK> *LI=NULL,*L1;
	if(FR_flag&UHASH_FRF_FIND)
		for(i=0,LI=L; (LI)&&(LI->key != Key); LI = LI->next,i++);
	if(LI==NULL)
	{
		LI = im.new_item();
		LI->key=Key;
		LI->data=value;
		if(LI->next = L)
			L->prev = LI;
		L=LI;
		ne++;
		return 0;
	}
	switch(FR_flag&FRF_DOIFEXIST)
	{
	case FRF_REPLACE:
		LI->data = value;
		return i;
	case FRF_ADDAFTER:
		L1 = im.new_item();
		L1->key=Key;
		L1->data=value;

		if(L1->next = LI->next)
			LI->next->prev = L1;
		L1->prev=LI;
		LI->next=L1;
		ne++;
		return i+1;
	case FRF_ADDBEFORE:
		L1 = im.new_item();
		L1->key=Key;
		L1->data=value;

		if(L1->prev = LI->prev)
			LI->prev->next = L1;
		else L=L1;
		L1->next=LI;
		LI->prev=L1;
		ne++;
		return i;
	}
	return -1;
}
//------------------------------------------------
template <typename TD, typename TK>
int L_List_f<TD,TK>::insert(const TK& Key, const TD* value, unsigned int FR_flag)
{
	int i;
	LL_item<TD,TK> *LI=NULL,*L1;
	if(FR_flag&UHASH_FRF_FIND)
		for(i=0,LI=L; (LI)&&(LI->key != Key); LI = LI->next,i++);
	if(LI==NULL)
	{
		LI = im.new_item();
		LI->key=Key;
		LI->data=*value;

		if(LI->next = L)
			L->prev = LI;
		L=LI;
		ne++;
		return 0;
	}
	if(value==NULL)return -1;
	switch(FR_flag&FRF_DOIFEXIST)
	{
	case FRF_REPLACE:
		LI->data = *value;
		return i;
	case FRF_ADDAFTER:
		L1 = im.new_item();
		L1->key=Key;
		L1->data=*value;

		if(L1->next = LI->next)
			LI->next->prev = L1;
		L1->prev=LI;
		LI->next=L1;
		ne++;
		return i+1;
	case FRF_ADDBEFORE:
		L1 = im.new_item();
		L1->key=Key;
		L1->data=*value;

		if(L1->prev = LI->prev)
			LI->prev->next = L1;
		else L=L1;
		L1->next=LI;
		LI->prev=L1;
		ne++;
		return i;
	}
	return -1;
}
//------------------------------------------------
template <typename TD, typename TK>
TD* L_List_f<TD,TK>::insert_adress(const TK &Key, const TD &value, unsigned int FR_flag)
{
	TD * d=insert_adress(Key, FR_flag);
	if(d) *d = value;
	return d;
}
//------------------------------------------------
template <typename TD, typename TK>
TD* L_List_f<TD,TK>::insert_adress(const TK &Key, unsigned int FR_flag)
{
	int i;
	LL_item<TD,TK> *LI=NULL,*L1;
	TD *res;
	if(FR_flag&UHASH_FRF_FIND)
		for(i=0,LI=L; (LI)&&(LI->key != Key); LI = LI->next,i++);
	if(LI==NULL)
	{
		LI = im.new_item();
		LI->key=Key;
		res=&(LI->data);
		if(LI->next = L)
			L->prev = LI;
		L=LI;
		ne++;
		return res;
	}
	switch(FR_flag&FRF_DOIFEXIST)
	{
	case FRF_REPLACE:
		LI->data = value;
		return &(LI->data);
	case FRF_ADDAFTER:
		L1 = im.new_item();
		L1->key=Key;
		res=&(L1->data);
		if(L1->next = LI->next)
			LI->next->prev = L1;
		ne++;
		L1->prev=LI;
		LI->next=L1;
		return res;
	case FRF_ADDBEFORE:
		L1 = im.new_item();
		L1->key=Key;
		res=&(L1->data);
		if(L1->prev = LI->prev)
			LI->prev->next = L1;
		else L=L1;
		ne++;
		L1->next=LI;
		LI->prev=L1;
		return res;
	}
	return &(LI->data);
}
//------------------------------------------------
template <typename TD, typename TK>
TD* L_List_f<TD,TK>::operator[](TK Key)
{
	return L_List_LT::get(Key);
}
//------------------------------------------------
template <typename TD, typename TK>
TD* L_List_f<TD,TK>::get(const TK& Key)
{
	return L_List_LT::get(Key);
}
//------------------------------------------------
template <typename TD, typename TK>
const TK* L_List_f<TD,TK>::get_key(const TK& Key) const
{
	return L_List_LT::get_key(Key);
}
//------------------------------------------------
template <typename TD, typename TK>
int L_List_f<TD,TK>::get(TK Key, TD &res, int N) const
{
	return L_List_LT::get(&Key, res, N);
}
//------------------------------------------------
template <typename TD, typename TK>
L_List_f<TD,TK>::~L_List_f()
{
	if(L!=NULL)
	{
		LL_item<TD,TK> *LI,*Next;
		for(LI=L; LI; LI=Next)
		{
			Next=LI->next;
			im.delete_item(LI);
		}
		L=last=NULL;
	}
}
//------------------------------------------------
template <typename TD, typename TK>
void L_List_f<TD,TK>::clear()
{
	if(L!=NULL)
	{
		LL_item<TD,TK> *LI,*Next;
		for(LI=L; LI; LI=Next)
		{
			Next=LI->next;
			im.delete_item(LI);
		}
		L=last=NULL;
	}
}
//------------------------------------------------
template <typename TD, typename TK>
int L_List_f<TD,TK>::delete_(TK Key,int N)
{
	int i,j,k=-1;
	LL_item<TD,TK> *t=L,*s=L;
	j=(N==LL_DELETEALL ? N : 0);
	for(i=0;(t!=NULL)&&(j<=N);i++)
	{
		t=s;
		s=t->next;
		if(t->key == Key)
		{
			if(N==LL_DELETEALL)
			{
				t->next=NULL;
				if(t->prev)t->prev->next=s;
				else L=s;
				if(s)s->prev=t->prev;
				im.delete_item(t);
				k++;
			}
			else j++;
		}
	}
	if(t)
	{
		s=t->next;
		t->next=NULL;
		if(t->prev)t->prev->next=s;
		else L=s;
		if(s)s->prev=t->prev;
		im.delete_item(t);
		return i;
	}
	if(k>=0)k++;
	return k;
}
//------------------------------------------------
template <typename TD, typename TK>
int L_List_f<TD,TK>::_delete(const TK& Key,int N)
{
	return delete_(Key,N);
}

///================================//
///=======CLASS L_KA_List_f==========//
template <typename TD, typename TK>
L_KA_List_f<TD,TK>::L_KA_List_f()
{
	L=NULL;
	ne=0;
	return;
}
//------------------------------------------------
template <typename TD, typename TK>
L_KA_List_f<TD,TK>::L_KA_List_f(const L_KA_List_f<TD,TK> &LL)
{
	LL_KA_item<TD,TK> *LI,**LI1=&L,*Prev=NULL;
	ne=LL.ne;
	for(LI=LL.L; LI; LI=LI->next,LI1=&((*LI1)->next))
	{
		*LI1=im.new_item();
		**LI1=*LI;
		(*LI1)->prev=Prev;
	}
	*LI1=NULL;
}
//------------------------------------------------
template <typename TD, typename TK>
L_KA_List_f<TD,TK>& L_KA_List_f<TD,TK>::operator=(const L_KA_List_f<TD,TK> &LL)
{
	ne=LL.ne;
	if(LL.L==NULL)
	{
		if(L!=NULL)delete L;
		L=NULL;
	}
	else  if(L!=NULL) *L=*LL.L;
	else 
	{
		LL_KA_item<TD,TK> *LI,**LI1=&L,*Prev=NULL;
		for(LI=LL.L; LI; LI=LI->next,LI1=&((*LI1)->next))
		{
			if(*LI1==NULL)*LI1 = im.new_item();
			**LI1 = *LI;
			(*LI1)->prev = Prev;
		}
		if(*LI1) im.delete_item(*LI1);
	}
	return *this;
}
//------------------------------------------------
template <typename TD, typename TK>
int L_KA_List_f<TD,TK>::insert(const TK &Key, const TD& value, unsigned int FR_flag)
{
	int i;
	LL_KA_item<TD,TK> *LI=NULL,*L1;
	if(FR_flag&UHASH_FRF_FIND)
		for(i=0,LI=L; (LI)&&(LI->key != Key); LI = LI->next,i++);
	if(LI==NULL)
	{
		LI = im.new_item();
		LI->key=Key;
		LI->data=value;
		if(LI->next = L)
			L->prev = LI;
		L=LI;
		ne++;
		return 0;
	}
	switch(FR_flag&FRF_DOIFEXIST)
	{
	case FRF_REPLACE:
		LI->data = value;
		return i;
	case FRF_ADDAFTER:
		L1 = im.new_item();
		L1->key=Key;
		L1->data=value;

		if(L1->next = LI->next)
			LI->next->prev = L1;
		L1->prev=LI;
		LI->next=L1;
		ne++;
		return i+1;
	case FRF_ADDBEFORE:
		L1 = im.new_item();
		L1->key=Key;
		L1->data=value;

		if(L1->prev = LI->prev)
			LI->prev->next = L1;
		else L=L1;
		L1->next=LI;
		LI->prev=L1;
		ne++;
		return i;
	}
	return -1;
}
//------------------------------------------------
template <typename TD, typename TK>
int L_KA_List_f<TD,TK>::insert(const TK& Key, const TD* value, unsigned int FR_flag)
{
	int i;
	LL_KA_item<TD,TK> *LI=NULL,*L1;
	if(FR_flag&UHASH_FRF_FIND)
		for(i=0,LI=L; (LI)&&(LI->key != Key); LI = LI->next,i++);
	if(LI==NULL)
	{
		LI = im.new_item();
		LI->key=Key;
		LI->data=*value;

		if(LI->next = L)
			L->prev = LI;
		L=LI;
		ne++;
		return 0;
	}
	if(value==NULL)return -1;
	switch(FR_flag&FRF_DOIFEXIST)
	{
	case FRF_REPLACE:
		LI->data = *value;
		return i;
	case FRF_ADDAFTER:
		L1 = im.new_item();
		L1->key=Key;
		L1->data=*value;

		if(L1->next = LI->next)
			LI->next->prev = L1;
		L1->prev=LI;
		LI->next=L1;
		ne++;
		return i+1;
	case FRF_ADDBEFORE:
		L1 = im.new_item();
		L1->key=Key;
		L1->data=*value;

		if(L1->prev = LI->prev)
			LI->prev->next = L1;
		else L=L1;
		L1->next=LI;
		LI->prev=L1;
		ne++;
		return i;
	}
	return -1;
}
//------------------------------------------------
template <typename TD, typename TK>
TD* L_KA_List_f<TD,TK>::insert_adress(const TK &Key, const TD &value, unsigned int FR_flag)
{
	TD * d = insert_adress(Key, FR_flag);
	if(d) *d = value;
	return d;
}
//------------------------------------------------
template <typename TD, typename TK>
TD* L_KA_List_f<TD,TK>::insert_adress(const TK &Key, unsigned int FR_flag)
{
	int i;
	LL_KA_item<TD,TK> *LI=NULL,*L1;
	TD *res;
	if(FR_flag&FRF_FIND)
		for(i=0,LI=L; (LI)&&(LI->key != Key); LI = LI->next,i++);
	if(LI==NULL)
	{
		LI = im.new_item();
		LI->key=Key;
	//	LI->data=value;
		res=&(LI->data);
		if(LI->next = L)
			L->prev = LI;
		L=LI;
		ne++;
		return res;
	}
	switch(FR_flag&FRF_DOIFEXIST)
	{
	case FRF_REPLACE:
//		LI->data = value;
		return &(LI->data);
	case FRF_ADDAFTER:
		L1 = im.new_item();
		L1->key=Key;
//		L1->data=value;
		res=&(L1->data);
		if(L1->next = LI->next)
			LI->next->prev = L1;
		ne++;
		L1->prev=LI;
		LI->next=L1;
		return res;
	case FRF_ADDBEFORE:
		L1 = im.new_item();
		L1->key=Key;
	//	L1->data=value;
		res=&(L1->data);
		if(L1->prev = LI->prev)
			LI->prev->next = L1;
		else L=L1;
		ne++;
		L1->next=LI;
		LI->prev=L1;
		return res;
	}
	return &(LI->data);
}
//------------------------------------------------
template <typename TD, typename TK>
TD* L_KA_List_f<TD,TK>::operator[](const TK& Key)
{
	return L_List_LT::get(Key);
}
//------------------------------------------------
template <typename TD, typename TK>
TD* L_KA_List_f<TD,TK>::get(const TK& Key)
{
	return L_List_LT::get(Key);
}
//------------------------------------------------
template <typename TD, typename TK>
const TK* L_KA_List_f<TD,TK>::get_key(const TK& Key) const
{
	return L_List_LT::get_key(Key);
}
//------------------------------------------------
template <typename TD, typename TK>
int L_KA_List_f<TD,TK>::get(const TK& Key, TD &res, int N)const
{
	return L_List_LT::get(&Key, res, N);
}
//------------------------------------------------
template <typename TD, typename TK>
L_KA_List_f<TD,TK>::~L_KA_List_f()
{
	if(L!=NULL)
	{
		LL_KA_item<TD,TK> *LI,*Next;
		for(LI=L; LI; LI=Next)
		{
			Next=LI->next;
			im.delete_item(LI);
		}
		L=last=NULL;
	}
}
//------------------------------------------------
template <typename TD, typename TK>
void L_KA_List_f<TD,TK>::clear()
{
	if(L!=NULL)
	{
		LL_KA_item<TD,TK> *LI,*Next;
		for(LI=L; LI; LI=Next)
		{
			Next=LI->next;
			im.delete_item(LI);
		}
		L=last=NULL;
	}
}
//------------------------------------------------
template <typename TD, typename TK>
int L_KA_List_f<TD,TK>::delete_(const TK& Key,int N)
{
	int i,j,k=-1;
	LL_KA_item<TD,TK> *t=L,*s=L;
	j=(N==LL_DELETEALL ? N : 0);
	for(i=0;(s!=NULL)&&(j<=N);i++)
	{
		t=s;
		s=t->next;
		if(t->key == Key)
		{
			if(N==LL_DELETEALL)
			{
				t->next=NULL;
				if(t->prev)t->prev->next=s;
				else L=s;
				if(s)s->prev=t->prev;
				im.delete_item(t);
				k++;
			}
			else j++;
		}
	}
	if(t)
	{
		s=t->next;
		t->next=NULL;
		if(t->prev)t->prev->next=s;
		else L=s;
		if(s)s->prev=t->prev;
		im.delete_item(t);
		return i;
	}
	if(k>=0)k++;
	return k;
}
//------------------------------------------------
template <typename TD, typename TK>
int L_KA_List_f<TD,TK>::_delete(const TK& Key,int N)
{
	return delete_(Key,N);
}
///===============================//
///=======CLASS L_KA_List==========//
//____ Linked list with a struct or a class as key type _______//
//------------------------------------------------
template <typename TD, typename TK>
L_KA_List<TD,TK>& L_KA_List<TD,TK>::operator=(const L_KA_List<TD,TK> &LL)
{
	DL_List_LT::operator =(LL);
	return *this;
}
//------------------------------------------------
template <typename TD, typename TK>
int L_KA_List<TD,TK>::insert(const TK& Key, const TD& value, unsigned int FR_flag)
{
	return DL_List_LT::insert(&Key, value, FR_flag);
}
//------------------------------------------------
template <typename TD, typename TK>
int L_KA_List<TD,TK>::insert(const TK& Key, const TD* value, unsigned int FR_flag)
{
	return DL_List_LT::insert(Key, value, FR_flag);
}

//------------------------------------------------
template <typename TD, typename TK>
TD* L_KA_List<TD,TK>::operator[](const TK& Key)
{
	return L_List_LT::get(Key);
}
//------------------------------------------------
template <typename TD, typename TK>
TD* L_KA_List<TD,TK>::get(const TK& Key)
{
	return L_List_LT::get(Key);
}
//------------------------------------------------
template <typename TD, typename TK>
int L_KA_List<TD,TK>::get(const TK& Key, TD &res,int N)const
{
	return L_List_LT::get(&Key, res, N);
}
//------------------------------------------------
template <typename TD, typename TK>
L_KA_List<TD,TK>::~L_KA_List(){}
//------------------------------------------------
template <typename TD, typename TK>
int L_KA_List<TD,TK>::delete_(const TK& Key,int N)
{
	return DL_List_LT::delete_(&Key, N);
}
//__________________________________________________
///==============================//
///=======CLASS Tree_item==========//
template <typename TD,typename TK>
Tree_item<TD,TK>* Tree_item<TD,TK>::nil=&Nil;//new Tree_item<TD,TK>();
//----------------------------------------
template <typename TD,typename TK>
Tree_item<TD,TK> Tree_item<TD,TK>::Nil;
//----------------------------------------
template <typename TD,typename TK>
const Tree_item<TD,TK>* Tree_item<TD,TK>::NIL()
{
	return nil;
}
//----------------------------------------
template <typename TD,typename TK>
Tree_item<TD,TK>::Tree_item()
{
	left=right=p=nil;
}
//----------------------------------------
template <typename TD,typename TK>
int Tree_item<TD,TK>::print(int (*printTK)(TK),int (*printTD)(const TD&))const 
{
	int res=0;
	res+=printf("(");
	if(left!=nil) res+=left->print(printTK , printTD);
	res+=printf(" ");
	res+=printTK(key);
	res+=printf("; ");
	res+=printTD(data);
	res+=printf(";");
	if(right!=nil) res+=right->print(printTK , printTD);
	res+=printf(")");
	return res;
}
//----------------------------------------
template <typename TD,typename TK>
int Tree_item<TD,TK>::print(FILE*fp, int (*printKD)(FILE*, const TK&, const TD&))const 
{
	int res=0;
	res+=fprintf(fp, "{");
	if(left!=nil) res+=left->print(fp, printKD);
	res+=printKD(fp, key, data);
	if(right!=nil) res+=right->print(fp, printKD);
	res+=printf("}");
	return res;
}
//----------------------------------------
template <typename TD,typename TK>
Tree_item<TD,TK>::Tree_item(const Tree_item<TD,TK> &Tree, Tree_item<TD,TK> *P)
{
	left=right=p=nil;
	key=Tree.key;
	data=Tree.data;
	if(Tree.left)
		left=new Tree_item(*Tree.left,this);
	if(Tree.right)
		right=new Tree_item(*Tree.right,this);
	if(P)p=P;
}

//----------------------------------------
template <typename TD,typename TK>
Tree_item<TD,TK>::Tree_item(TK Key0, const TD& value, Tree_item<TD,TK> *P)
{
	left=right=p=nil;
	key=Key0;
	data=value;
	if(P)p=P;
}
//----------------------------------------
template <typename TD,typename TK>
Tree_item<TD,TK>::Tree_item(TK Key0, const TD* value, Tree_item<TD,TK> *P)
{
	left=right=p=nil;
	key=Key0;
	if(value)data = *value;
	if(P)p=P;
}
//----------------------------------------
template <typename TD,typename TK>
void Tree_item<TD,TK>::init(TK Key0, const TD& value)
{
	left=right=p=nil;
	key=Key0;
	data=value;
}

//----------------------------------------
template <typename TD,typename TK>
Tree_item<TD,TK>::~Tree_item()
{
	if(left)T_DELETE(left);
	if(right)T_DELETE(right);
	if(p)p=nil;
}
//----------------------------------------
template <typename TD,typename TK>
Tree_item<TD,TK>& Tree_item<TD,TK>::operator =(const Tree_item<TD,TK> &Tree)
{
	if((this==nil)||(this==&Tree)||(&Tree==nil))return *this;
	p=nil;
	key=Tree.key;
	data=Tree.data;
	if(Tree.left!=nil)
		if(left!=nil)*left=*Tree.left;
		else left=new Tree_item(*Tree.left,this);
	else T_DELETE(left);
	if(Tree.right!=nil)
		if(right!=nil)*right=*Tree.right;
		else right=new Tree_item(*Tree.right,this);
	else T_DELETE(right);
	return *this;
}

//----------------------------------------
template <typename TD,typename TK>
TD& Tree_item<TD,TK>::get_data()
{
	return data;
}
//----------------------------------------
template <typename TD,typename TK>
TK Tree_item<TD,TK>::get_key() const
{
	return key;
}

///=============================//
///=======CLASS RB_item==========//
template <typename TD,typename TK>
RB_item<TD,TK>* RB_item<TD,TK>::nil=&Nil;//RB_item<TD,TK>();
//----------------------------------------
template <typename TD,typename TK>
RB_item<TD,TK> RB_item<TD,TK>::Nil;
//----------------------------------------
template <typename TD,typename TK>
const RB_item<TD,TK>* RB_item<TD,TK>::NIL()
{
	return nil;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item<TD,TK>::RB_item()
{
	left=right=p=nil;
	color=RBT_BLACK;
}
//----------------------------------------
template <typename TD,typename TK>
int RB_item<TD,TK>::print(FILE *fp, int (*printTK)(FILE*,const TK&),int (*printTD)(FILE*,const TD&))const 
{
	int res=0;
	res+=fprintf(fp,"(");
	if(left!=nil) res+=left->print(fp, printTK , printTD);
	res+=fprintf(fp, "|");
	res+=printTK(fp, key);
	res+=fprintf(fp," ");
	res+=printTD(fp, data);
	res+=fprintf(fp, " %s|",(color==RBT_RED ? "red" : "black"));
	res+=fprintf(fp, " ");
	if(right!=nil) res+=right->print(fp, printTK , printTD);
	res+=fprintf(fp, ")");
	return res;
}
//----------------------------------------
template <typename TD,typename TK>
int RB_item<TD,TK>::print(FILE*fp, int (*printKD)(FILE*, const TK&, const TD&))const 
{
	int res=0;
	res+=fprintf(fp, "{");
	if(left!=nil) res+=left->print(fp, printKD);
	res+=printKD(fp, key, data);
	if(right!=nil) res+=right->print(fp, printKD);
	res+=printf("}");
	return res;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item<TD,TK>::RB_item(const RB_item<TD,TK> &Tree, RB_item<TD,TK> *P)
{
	left=right=p=nil;
	key=Tree.key;
	data=Tree.data;
	color=Tree.color;
	if(Tree.left)
		left=new RB_item(*Tree.left,this);
	if(Tree.right)
		right=new RB_item(*Tree.right,this);
	if(P)p=P;
}

//----------------------------------------
template <typename TD,typename TK>
RB_item<TD,TK>::RB_item(TK Key0, const TD& value,char Color, RB_item<TD,TK> *P)
{
	left=right=p=nil;
	key=Key0;
	data=value;
	color=Color;
	if(P)p=P;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item<TD,TK>::RB_item(TK Key0, const TD* value, RB_item<TD,TK> *P)
{
	left=right=p=nil;
	key=Key0;
	color=RBT_BLACK;
	if(value)data = *value;
	if(P)p=P;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item<TD,TK>::RB_item(TK Key0, const TD* value,char Color, RB_item<TD,TK> *P)
{
	left=right=p=nil;
	key=Key0;
	color=Color;
	if(value)data = *value;
	if(P)p=P;
}
//----------------------------------------
template <typename TD,typename TK>
void RB_item<TD,TK>::init(TK Key0, const TD& value,char Color)
{
	left=right=p=nil;
	key=Key0;
	data=value;
	color=Color;
}

//----------------------------------------
template <typename TD,typename TK>
RB_item<TD,TK>::~RB_item()
{
	if(left)T_DELETE(left);
	if(right)T_DELETE(right);
	color=RBT_BLACK;
	if(p)p=nil;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item<TD,TK>& RB_item<TD,TK>::operator =(const RB_item<TD,TK> &Tree)
{
	if((this==nil)||(this==&Tree)||(&Tree==nil))return *this;
	p=nil;
	key=Tree.key;
	data=Tree.data;
	color=Tree.color;
	if(Tree.left!=nil)
		if(left!=nil)*left=*Tree.left;
		else left=new RB_item<TD,TK>(*Tree.left,this);
	else T_DELETE(left);
	if(Tree.right!=nil)
		if(right!=nil)*right=*Tree.right;
		else right=new RB_item<TD,TK>(*Tree.right,this);
	else T_DELETE(right);
	return *this;
}
//----------------------------------------
template <typename TD,typename TK>
TD& RB_item<TD,TK>::get_data()
{
	return data;
}
//----------------------------------------
template <typename TD,typename TK>
TK RB_item<TD,TK>::get_key() const
{
	return key;
}

///=================================//
///=======CLASS RB_item_KA==========//
template <typename TD,typename TK>
RB_item_KA<TD,TK>* RB_item_KA<TD,TK>::nil=&Nil;//new RB_item_KA<TD,TK>();
//----------------------------------------
template <typename TD,typename TK>
RB_item_KA<TD,TK> RB_item_KA<TD,TK>::Nil;
//----------------------------------------
template <typename TD,typename TK>
const RB_item_KA<TD,TK>* RB_item_KA<TD,TK>::NIL()
{
	return nil;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item_KA<TD,TK>::RB_item_KA()
{
	left=right=p=nil;
	color=RBT_BLACK;
}
//----------------------------------------
template <typename TD,typename TK>
int RB_item_KA<TD,TK>::print(FILE *fp, int (*printTK)(FILE*,const TK&),int (*printTD)(FILE*,const TD&))const 
{
	int res=0;
	res+=fprintf(fp,"(");
	if(left!=nil) res+=left->print(fp, printTK , printTD);
	res+=fprintf(fp, "|");
	res+=printTK(fp, key);
	res+=fprintf(fp," ");
	res+=printTD(fp, data);
	res+=fprintf(fp, " %s|",(color==RBT_RED ? "red" : "black"));
	res+=fprintf(fp, " ");
	if(right!=nil) res+=right->print(fp, printTK , printTD);
	res+=fprintf(fp, ")");
	return res;
}
//----------------------------------------
template <typename TD,typename TK>
int RB_item_KA<TD,TK>::print(FILE*fp, int (*printKD)(FILE*, const TK&, const TD&))const 
{
	int res=0;
	res+=fprintf(fp, "{");
	if(left!=nil) res+=left->print(fp, printKD);
	res+=printKD(fp, key, data);
	if(right!=nil) res+=right->print(fp, printKD);
	res+=fprintf(fp, "}");
	return res;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item_KA<TD,TK>::RB_item_KA(const RB_item_KA<TD,TK> &Tree, RB_item_KA<TD,TK> *P)
{
	left=right=p=nil;
	key=Tree.key;
	data=Tree.data;
	color=Tree.color;
	if(Tree.left)
		left=new RB_item_KA(*Tree.left,this);
	if(Tree.right)
		right=new RB_item_KA(*Tree.right,this);
	if(P)p=P;
}

//----------------------------------------
template <typename TD,typename TK>
RB_item_KA<TD,TK>::RB_item_KA(const TK& Key0, const TD& value,char Color, RB_item_KA<TD,TK> *P)
{
	left=right=p=nil;
	key=Key0;
	data=value;
	color=Color;
	if(P)p=P;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item_KA<TD,TK>::RB_item_KA(const TK& Key0, const TD* value, RB_item_KA<TD,TK> *P)
{
	left=right=p=nil;
	key=Key0;
	color=RBT_BLACK;
	if(value)data = *value;
	if(P)p=P;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item_KA<TD,TK>::RB_item_KA(const TK& Key0, const TD* value,char Color, RB_item_KA<TD,TK> *P)
{
	left=right=p=nil;
	key=Key0;
	color=Color;
	if(value)data = *value;
	if(P)p=P;
}
//----------------------------------------
template <typename TD,typename TK>
void RB_item_KA<TD,TK>::init(const TK& Key0, const TD& value,char Color)
{
	left=right=p=nil;
	key=Key0;
	data=value;
	color=Color;
}

//----------------------------------------
template <typename TD,typename TK>
RB_item_KA<TD,TK>::~RB_item_KA()
{
	if(left)T_DELETE(left);
	if(right)T_DELETE(right);
	color=RBT_BLACK;
	if(p)p=nil;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item_KA<TD,TK>& RB_item_KA<TD,TK>::operator =(const RB_item_KA<TD,TK> &Tree)
{
	if((this==nil)||(this==&Tree)||(&Tree==nil))return *this;
	p=nil;
	key=Tree.key;
	data=Tree.data;
	color=Tree.color;
	if(Tree.left!=nil)
		if(left!=nil)*left=*Tree.left;
		else left=new RB_item_KA<TD,TK>(*Tree.left,this);
	else T_DELETE(left);
	if(Tree.right!=nil)
		if(right!=nil)*right=*Tree.right;
		else right=new RB_item_KA<TD,TK>(*Tree.right,this);
	else T_DELETE(right);
	return *this;
}
//----------------------------------------
template <typename TD,typename TK>
TD& RB_item_KA<TD,TK>::get_data()
{
	return data;
}
//----------------------------------------
template <typename TD,typename TK>
const TK& RB_item_KA<TD,TK>::get_key() const
{
	return key;
}

///=================================//
///=======CLASS TreeItem_KA==========//
template <typename TD,typename TK>
TreeItem_KA<TD,TK>* TreeItem_KA<TD,TK>::nil=&Nil;//new TreeItem_KA<TD,TK>;
//----------------------------------------
template <typename TD,typename TK>
TreeItem_KA<TD,TK> TreeItem_KA<TD,TK>::Nil;
//----------------------------------------
template <typename TD,typename TK>
const TreeItem_KA<TD,TK>* TreeItem_KA<TD,TK>::NIL()
{
	return nil;
}
//----------------------------------------
template <typename TD,typename TK>
TreeItem_KA<TD,TK>::TreeItem_KA()
{
	left=right=p=nil;
}
//----------------------------------------
template <typename TD,typename TK>
int TreeItem_KA<TD,TK>::print(int (*printTK)(const TK&),int (*printTD)(const TD&))const 
{
	int res=0;
	res+=printf("(");
	if(left!=nil) res+=left->print(printTK , printTD);
	res+=printf(" ");
	res+=printTK(key);
	res+=printf("; ");
	res+=printTD(data);
	res+=printf(";");
	if(right!=nil) res+=right->print(printTK , printTD);
	res+=printf(")");
	return res;
}
//----------------------------------------
template <typename TD,typename TK>
int TreeItem_KA<TD,TK>::print(FILE*fp, int (*printKD)(FILE*, const TK&, const TD&))const 
{
	int res=0;
	res+=fprintf(fp, "{");
	if(left!=nil) res+=left->print(fp, printKD);
	res+=printKD(fp, key, data);
	if(right!=nil) res+=right->print(fp, printKD);
	res+=fprintf(fp, "}");
	return res;
}
//----------------------------------------
template <typename TD,typename TK>
TreeItem_KA<TD,TK>::TreeItem_KA(const TreeItem_KA<TD,TK> &Tree, TreeItem_KA<TD,TK> *P)
{
	left=right=p=nil;
	key=Tree.key;
	data=Tree.data;
	if(Tree.left)
		left=new TreeItem_KA(*Tree.left,this);
	if(Tree.right)
		right=new TreeItem_KA(*Tree.right,this);
	if(P)p=P;
}

//----------------------------------------
template <typename TD,typename TK>
TreeItem_KA<TD,TK>::TreeItem_KA(const TK& Key0, const TD& value, TreeItem_KA<TD,TK> *P)
{
	left=right=p=nil;
	key=Key0;
	data=value;
	if(P)p=P;
}
//----------------------------------------
template <typename TD,typename TK>
TreeItem_KA<TD,TK>::TreeItem_KA(const TK& Key0, const TD* value, TreeItem_KA<TD,TK> *P)
{
	left=right=p=nil;
	key=Key0;
	if(value)data = *value;
	if(P)p=P;
}
//----------------------------------------
template <typename TD,typename TK>
void TreeItem_KA<TD,TK>::init(const TK& Key0, const TD& value)
{
	left=right=p=nil;
	key=Key0;
	data=value;
}

//----------------------------------------
template <typename TD,typename TK>
TreeItem_KA<TD,TK>::~TreeItem_KA()
{
	if(left)T_DELETE(left);
	if(right)T_DELETE(right);
	if(p)p=nil;
}
//----------------------------------------
template <typename TD,typename TK>
TreeItem_KA<TD,TK>& TreeItem_KA<TD,TK>::operator =(const TreeItem_KA<TD,TK> &Tree)
{
	if((this==nil)||(this==&Tree)||(&Tree==nil))return *this;
	p=nil;
	key=Tree.key;
	data=Tree.data;
	if(Tree.left!=nil)
		if(left!=nil)*left=*Tree.left;
		else left=new TreeItem_KA(*Tree.left,this);
	else T_DELETE(left);
	if(Tree.right!=nil)
		if(right!=nil)*right=*Tree.right;
		else right=new TreeItem_KA(*Tree.right,this);
	else T_DELETE(right);
	return *this;
}
//----------------------------------------
template <typename TD,typename TK>
TD& TreeItem_KA<TD,TK>::get_data()
{
	return data;
}
//----------------------------------------
template <typename TD,typename TK>
const TK& TreeItem_KA<TD,TK>::get_key() const
{
	return key;
}

///===============================//
///=======CLASS BinaryTree==========//
template <typename TD,typename TK, class TI>
TI*& BinaryTree<TD,TK,TI>::find(const TK& Key, TI** P)
{
	TI **T=&root,*TPrev=nil;
	while(*T!=nil)
	{
		if((*T)->key == Key)
		{
			if(P) *P=(*T)->p;
			return T[0];
		}
		TPrev=*T;
		if((*T)->key > Key) T=&((*T)->right);
		else T=&((*T)->left);
	}
	if(P) *P=TPrev;
	return *T;
}
//----------------------------------------
template <typename TD,typename TK, class TI>
TI*const& BinaryTree<TD,TK,TI>::find_c(const TK& Key, const TI** P) const
{
	TI *const *T = &root;
	const TI* TPrev=nil;
	while(*T!=nil)
	{
		if((*T)->key == Key)
		{
			if(P) *P=(*T)->p;
			return T[0];
		}
		TPrev=*T;
		if((*T)->key > Key) T=&((*T)->right);
		else T=&((*T)->left);
	}
	if(P) *P=TPrev;
	return *T;
}
//----------------------------------------
template <typename TD, typename TK, class TI>
TI* BinaryTree<TD, TK, TI>::predecessor(TI *X)
{
	TI* Y;
	if(X->left!=nil) return minimum(X->left);
	Y=X->p;
	while((Y!=nil)&&(X==Y->left))
	{
		X=Y;
		Y=Y->p;
	}
	return Y;
}
//----------------------------------------
template <typename TD, typename TK, class TI>
TI* BinaryTree<TD, TK, TI>::successor(TI *X)
{
	TI* Y;
	if(X->right!=nil) return minimum(X->right);
	Y=X->p;
	while((Y!=nil)&&(X==Y->right))
	{
		X=Y;
		Y=Y->p;
	}
	return Y;
}
//----------------------------------------
template <typename TD, typename TK, class TI>
inline TI* BinaryTree<TD, TK, TI>::minimum(TI *X)
{
	if(!X)X=root;
	else	if(X==nil)return X;
	while(X->left!=nil)
		X=X->left;
	return X;
}
//----------------------------------------
template <typename TD, typename TK, class TI>
inline TI* BinaryTree<TD, TK, TI>::maximum(TI *X)
{
	if(!X)X=root;
	else	if(X==nil)return X;
	while(X->right!=nil)
		X=X->right;
	return X;
}
//----------------------------------------
template <typename TD, typename TK, class TI>
int BinaryTree<TD, TK, TI>::left_rotate(TI *X)
{
	TI* Y = X->right;
	if(Y==nil)return -1;
	X->right = Y->left;
	if(Y->left!=nil) Y->left->p = X;
	Y->p=X->p;
	if(X->p==nil) root = Y;
	else if(X==X->p->left) X->p->left = Y;
	else	X->p->right = Y;
	Y->left=X;
	X->p=Y;
	return 0;
}

//----------------------------------------
template <typename TD, typename TK, class TI>
int BinaryTree<TD, TK, TI>::right_rotate(TI *X)
{
	TI *Y = X->left;
	if(Y==nil)return -1;
	X->left = Y->right;
	if(Y->right!=nil) Y->right->p = X;
	Y->p=X->p;
	if(X->p==nil) root = Y;
	else if(X==X->p->left) X->p->left = Y;
	else	X->p->right = Y;
	Y->right=X;
	X->p=Y;
	return 0;
}
//----------------------------------------
template <typename TD, typename TK, class TI>
BinaryTree<TD,TK,TI>& BinaryTree<TD, TK, TI>::operator=(const BinaryTree<TD,TK,TI> &Tree)
{
	if(Tree.root==nil)
	{
		T_DELETE(root);
		return *this;
	}
	if((root!=nil)&&(root)) *root=*Tree.root;
	else root=new TI(*Tree.root,NULL);
	return *this;
}
//----------------------------------------
template <typename TD, typename TK, class TI>
int BinaryTree<TD, TK, TI>::insert(const TK &Key, const TD *value, unsigned int FR_flag=FRF_FIND | FRF_REPLACE)
{
	TI **Curr,*P;
	Curr=&find(Key,&P);
	if(*Curr!=nil)
	{	
		if(value==NULL)return -1;
		switch(FR_flag & FRF_DOIFEXIST)
		{
		case FRF_REPLACE:
			(*Curr)->data=*value;
			return 1;
		case FRF_ADDBEFORE:
			while(((*Curr)->left!=nil)&&((*Curr)->left->key==Key))
				Curr=&(*Curr)->left;
			if((*Curr)->left!=nil)
			{
				P=(*Curr)->left;
				(*Curr)->left=new TI(Key,value,*Curr);
				(*Curr)->left->left=P;
				if(P)P->p=(*Curr)->left;
				return 1;
			}
			(*Curr)->left=new TI(Key,value,*Curr);
			return 1;
		case FRF_ADDAFTER:
			while(((*Curr)->right!=nil)&&((*Curr)->right->key==Key))
				Curr=&(*Curr)->right;
			if((*Curr)->right!=nil)
			{
				P=(*Curr)->right;
				(*Curr)->right=new TI(Key,value,*Curr);
				(*Curr)->right->right=P;
				if(P)P->p=(*Curr)->right;
				return 1;
			}
			(*Curr)->right=new TI(Key,value,*Curr);
			return 1;

#if (defined _ADD_VALUE ) 

		case FRF_ADDTODATA:
			(*Curr)->data.add_value(value);
			return 0;
#endif

		}
		return -1;
	}
	if(FR_flag & FRF_ONLYEXISTING)return -1;
	(*Curr)=new TI(Key,value,P);
	return 1;
}
//----------------------------------------
template <typename TD, typename TK, class TI>
TD* BinaryTree<TD, TK, TI>::insert_adress(const TK &Key, const TD &value, unsigned int FR_flag=FRF_FIND | FRF_REPLACE)
{
	TI **Curr, *P;
	Curr=&find(Key,&P);
	if(*Curr!=nil)
	{	
		switch(FR_flag & FRF_DOIFEXIST)
		{
		case FRF_REPLACE:
			(*Curr)->data=value;
			return &((*Curr)->data);
		case FRF_ADDBEFORE:
			while(((*Curr)->left!=nil)&&((*Curr)->left->key==Key))
				Curr=&(*Curr)->left;
			Curr=&(*Curr)->left;
			if((*Curr)!=nil)
			{
				P=(*Curr);
				(*Curr)=new TI(Key, &value, (*Curr)->p);
				(*Curr)->left=P;
				if(P)P->p=(*Curr);
				return &((*Curr)->data);
			}
			(*Curr)=new TI(Key, &value, (*Curr)->p);
			return &((*Curr)->data);
		case FRF_ADDAFTER:
			while(((*Curr)->right!=nil)&&((*Curr)->right->key==Key))
				Curr=&(*Curr)->right;
			if((*Curr)->right!=nil)
			{
				P=(*Curr)->right;
				(*Curr)->right=new TI(Key,&value,*Curr);
				(*Curr)->right->right=P;
				if(P)P->p=(*Curr)->right;
				return &((*Curr)->right->data);
			}
			(*Curr)->right=new TI(Key, &value, *Curr);
			return &((*Curr)->right->data);

#if (defined _ADD_VALUE ) 

		case FRF_ADDTODATA:
			(*Curr)->data.add_value(value);
			return 0;
#endif

		}
		return &((*Curr)->data);
	}
	if(FR_flag & FRF_ONLYEXISTING)return NULL;
	(*Curr)=new TI(Key, &value, P);
	return &((*Curr)->data);
}
//----------------------------------------
template <typename TD, typename TK, class TI>
TD* BinaryTree<TD, TK, TI>::insert_adress(const TK &Key, unsigned int FR_flag=FRF_FIND | FRF_REPLACE)
{
	TI **Curr, *P;
	Curr=&find(Key,&P);
	if(*Curr!=nil)
	{	
		switch(FR_flag & FRF_DOIFEXIST)
		{
		case FRF_REPLACE:
//			(*Curr)->data=value;
			return &((*Curr)->data);
		case FRF_ADDBEFORE:
			while(((*Curr)->left!=nil)&&((*Curr)->left->key==Key))
				Curr=&(*Curr)->left;
			Curr=&(*Curr)->left;
			if((*Curr)!=nil)
			{
				P=(*Curr);
				(*Curr)=new TI(Key, NULL, (*Curr)->p);
				(*Curr)->left=P;
				if(P)P->p=(*Curr);
				return &((*Curr)->data);
			}
			(*Curr)=new TI(Key, NULL, (*Curr)->p);
			return &((*Curr)->data);
		case FRF_ADDAFTER:
			while(((*Curr)->right!=nil)&&((*Curr)->right->key==Key))
				Curr=&(*Curr)->right;
			if((*Curr)->right!=nil)
			{
				P=(*Curr)->right;
				(*Curr)->right=new TI(Key,NULL,*Curr);
				(*Curr)->right->right=P;
				if(P)P->p=(*Curr)->right;
				return &((*Curr)->right->data);
			}
			(*Curr)->right=new TI(Key, NULL, *Curr);
			return &((*Curr)->right->data);

#if (defined _ADD_VALUE ) 

		case FRF_ADDTODATA:
			(*Curr)->data.add_value(value);
			return 0;
#endif

		}
		return &((*Curr)->data);
	}
	if(FR_flag & FRF_ONLYEXISTING)return NULL;
	(*Curr)=new TI(Key, NULL, P);
	return &((*Curr)->data);
}
//----------------------------------------
template <typename TD, typename TK, class TI>
int BinaryTree<TD, TK, TI>::_delete(const TK &Key, int N)
{
	TI *Y=NULL,*Z,*X,*P;
	int i;
	for(i=0,Z=find(Key,&P); (Z!=nil)&&((i<=N)||(N==LL_DELETEALL)); Z=find(Key,&P),i++)
	{
		if((Z->right==nil)||(Z->left==nil))Y=Z;
		else Y=successor(Z);
	
		if(Y->left!=nil) X = Y->left;
		else  X = Y->right;
	
		if((X->p = Y->p) == nil) root = X;
		else if(Y==Y->p->left) Y->p->left = X;
		else Y->p->right = X;
		if(Y!=Z)
		{
			Z->key = Y->key;
			Z->data = Y->data;
		}
		nil->p=NULL;
		Y->left = Y->right = Y->p = nil;
	}
	if(i)return i;
	return -1;
}
//----------------------------------------
template <typename TD, typename TK, class TI>
TD* BinaryTree<TD, TK, TI>::get(const TK &Key)
{
	TI* ti=find(Key);
	if(ti!=nil)return &(ti->data);
	else return NULL;
}
//----------------------------------------
template <typename TD, typename TK, class TI>
const TK* BinaryTree<TD, TK, TI>::get_key(const TK &Key)const
{
	const TI* ti=find_c(Key);
	if(ti!=nil)return &(ti->key);
	else return NULL;
}
//----------------------------------------
template <typename TD, typename TK, class TI>
void BinaryTree<TD, TK, TI>::clear()
{
	T_DELETE(root);
	ne=0;
}
//----------------------------------------
template <typename TD, typename TK, class TI>
BinaryTree<TD, TK, TI>::~BinaryTree()
{
	T_DELETE(root);
}
//----------------------------------------
template <typename TD,typename TK, class TI>
int BinaryTree<TD,TK,TI>::print(FILE *fp, int (*printKD)(FILE*, const TK&, const TD&))const
{
	if(root!=nil)
		return root->print(fp, printKD);
	return fprintf(fp, "<empty>");
}

//_________________________________________________________
///================================//
///=======CLASS Binary_Tree==========//
template <typename TD,typename TK>
Binary_Tree<TD,TK>::Binary_Tree()
{
	nil=root=Tree_item<TD,TK>::nil;
	ne=0;
}

//----------------------------------------
template<typename TD,typename TK>
Tree_item<TD,TK>** Find(TK Key, Binary_Tree<TD,TK> *&T0)
{
	int R=1;
	Tree_item<TD,TK> **T=,*Tprev=T0;
	if(!T0->cmp)return &T0;

	while(*T!=NULL)
	{
		if(((*T)->color&RBT_UNUSED)||((R=T0->cmp((*T)->key,Key))==0))return T;
		if(R==1)T=&((*T)->right);
		else if(R==-1)T=&((*T)->left);
			else return NULL;

	}
	return T;
}

//----------------------------------------
template <typename TD,typename TK>
Tree_item<TD,TK>*& Binary_Tree<TD,TK>::Find(TK Key, Tree_item<TD,TK>** P)
{
	Tree_item<TD,TK> **T=&root,*TPrev=nil;
	while(*T!=nil)
	{
		if((*T)->key == Key)
		{
			if(P) *P=(*T)->p;
			return T[0];
		}
		TPrev=*T;
		if((*T)->key > Key) T=&((*T)->right);
		else T=&((*T)->left);
	}
	if(P) *P=TPrev;
	return *T;
}
//----------------------------------------
template <typename TD,typename TK>
inline int Binary_Tree<TD,TK>::left_rotate(TK key)
{
	return BinaryTree::left_rotate(find(key,NULL));
}
//----------------------------------------
template <typename TD,typename TK>
int Binary_Tree<TD,TK>::right_rotate(TK key)
{
	return BinaryTree::right_rotate(find(key,NULL));
}
//----------------------------------------
template <typename TD,typename TK>
Binary_Tree<TD,TK>::Binary_Tree(const Binary_Tree<TD,TK> &Tree)
{
	nil=Tree_item<TD,TK>::nil;
	if(Tree.root==nil)root=nil;
	else root=new Tree_item<TD,TK>(Tree->root,NULL);
	ne=Tree.ne;
}

//----------------------------------------
template <typename TD,typename TK>
Binary_Tree<TD,TK>& Binary_Tree<TD,TK>::operator=(const Binary_Tree<TD,TK>& Tree)
{
	if(Tree.root==nil)
	{
		T_DELETE(root);
		ne=0;
		return *this;
	}
	if((root!=nil)&&(root)) *root=*Tree.root;
	else root=new Tree_item<TD,TK>(*Tree.root,NULL);
	ne=Tree.ne;
	return *this;
}

//----------------------------------------
template <typename TD,typename TK>
Binary_Tree<TD,TK>::~Binary_Tree()
{
	T_DELETE(root);
}
//----------------------------------------
template <typename TD,typename TK>
Tree_item<TD,TK>* Binary_Tree<TD,TK>::insert(TK Key, const TD& value,unsigned int FR_flag)
{
	Tree_item<TD,TK> **Curr,*P;
	Curr=&Find(Key,&P);
	if((*Curr!=nil))
	{	
		switch(FR_flag & FRF_DOIFEXIST)
		{
		case FRF_REPLACE:
			(*Curr)->data=value;
			return *Curr;
		case FRF_ADDBEFORE:
			while(((*Curr)->left!=nil)&&((*Curr)->left->key==Key))
				Curr=&(*Curr)->left;
			if((*Curr)->left!=nil)
			{
				P=(*Curr)->left;
				(*Curr)->left=new Tree_item<TD,TK>(Key,value,*Curr);
				(*Curr)->left->left=P;
				if(P)P->p=(*Curr)->left;
				return (*Curr)->left;
			}
			(*Curr)->left=new Tree_item<TD,TK>(Key,value,*Curr);
			return (*Curr)->left;
		case FRF_ADDAFTER:
			while(((*Curr)->right!=nil)&&((*Curr)->right->key==Key))
				Curr=&(*Curr)->right;
			if((*Curr)->right!=nil)
			{
				P=(*Curr)->right;
				(*Curr)->right=new Tree_item<TD,TK>(Key,value,*Curr);
				(*Curr)->right->right=P;
				if(P)P->p=(*Curr)->right;
				return (*Curr)->right;
			}
			(*Curr)->right=new Tree_item<TD,TK>(Key,value,*Curr);
			return (*Curr)->right;
		}
		return *Curr;
	}
	if(FR_flag & FRF_ONLYEXISTING)return NULL;
	(*Curr)=new Tree_item<TD,TK>(Key,value,P);
	ne++;
	return *Curr;
}
//----------------------------------------
/*template <typename TD,typename TK>
Binary_Tree<TD,TK>& Binary_Tree<TD,TK>::operator=(const Binary_Tree<TD, TK> &T)
{
	BinaryTree::operator=(T);
	return *this;
}*/
//----------------------------------------
template <typename TD,typename TK>
Tree_item<TD,TK>* Binary_Tree<TD,TK>::delete_(TK Key)
{
	int R=-1;
	Tree_item<TD,TK> *Y,*Z,*X,*P;
	Z=Find(Key,&P);
	if(Z==nil)return NULL;
	
	if((Z->right==nil)||(Z->left==nil))Y=Z;
	else Y=successor(Z);
	
	if(Y->left!=nil) X = Y->left;
	else  X = Y->right;
	
	if((X->p = Y->p) == nil) root = X;
	else if(Y==Y->p->left) Y->p->left = X;
	else Y->p->right = X;
	if(Y!=Z)
	{
		Z->key = Y->key;
		Z->data = Y->data;
	}
	nil->p=NULL;
	Y->left = Y->right = Y->p = nil;
	ne--;
	return Y;
}
//----------------------------------------
template <typename TD,typename TK>
TD* Binary_Tree<TD,TK>::operator[](TK Key)
{
	Tree_item<TD,TK>* TI=Find(Key);
	if(TI!=nil)return &(TI->data);
	else return NULL;
}
//----------------------------------------
/*template <typename TD,typename TK>
int Binary_Tree<TD,TK>::print(int (*printTK)(TK), int (*printTD)(const TD&))const 
{
	if(root!=nil)
		return (root->print(printTK,printTD)+printf("\n"));
	return printf("empty\n");
}*/
///================================//
///=======CLASS BinaryTree_KA========//
template <typename TD,typename TK>
BinaryTree_KA<TD,TK>::BinaryTree_KA()
{
	root=TreeItem_KA<TD,TK>::nil;
	nil=TreeItem_KA<TD,TK>::nil;
	ne=0;
}

//----------------------------------------
template<typename TD,typename TK>
TreeItem_KA<TD,TK>** Find(const TK& Key,BinaryTree_KA<TD,TK> *&T0)
{
	int R=1;
	TreeItem_KA<TD,TK> **T=,*Tprev=T0;
	if(!T0->cmp)return &T0;

	while(*T!=NULL)
	{
		if(((*T)->color&RBT_UNUSED)||((R=T0->cmp((*T)->key,Key))==0))return T;
		if(R==1)T=&((*T)->right);
		else if(R==-1)T=&((*T)->left);
			else return NULL;

	}
	return T;
}

//----------------------------------------
/*template <typename TD,typename TK>
TreeItem_KA<TD,TK>*& BinaryTree_KA<TD,TK>::Find(const TK& Key, TreeItem_KA<TD,TK>** P)
{
	TreeItem_KA<TD,TK> **T=&root,*TPrev=nil;
	while(*T!=nil)
	{
		if((*T)->key == Key)
		{
			if(P) *P=(*T)->p;
			return T[0];
		}
		TPrev=*T;
		if((*T)->key > Key) T=&((*T)->right);
		else T=&((*T)->left);
	}
	if(P) *P=TPrev;
	return *T;
}*/
//----------------------------------------
template <typename TD,typename TK>
inline int BinaryTree_KA<TD,TK>::left_rotate(const TK& key)
{
	return BinaryTree::left_rotate(find(key,NULL));
}
//----------------------------------------
template <typename TD,typename TK>
int BinaryTree_KA<TD,TK>::right_rotate(const TK& key)
{
	return BinaryTree::right_rotate(find(key,NULL));
}
//----------------------------------------
template <typename TD,typename TK>
BinaryTree_KA<TD,TK>::BinaryTree_KA(const BinaryTree_KA<TD,TK> &Tree)
{
	nil=TreeItem_KA<TD,TK>::nil;
	if(Tree.root==nil)root=nil;
	else root=new TreeItem_KA<TD,TK>(*Tree.root,NULL);
	ne=Tree.ne;
}

//----------------------------------------
template <typename TD,typename TK>
BinaryTree_KA<TD,TK>& BinaryTree_KA<TD,TK>::operator=(const BinaryTree_KA<TD,TK>& Tree)
{
	if(Tree.root==nil)
	{
		T_DELETE(root);
		return *this;
	}
	if((root!=nil)&&(root)) *root=*Tree.root;
	else root=new TreeItem_KA<TD,TK>(*Tree.root,NULL);
	return *this;
}

//----------------------------------------
template <typename TD,typename TK>
BinaryTree_KA<TD,TK>::~BinaryTree_KA()
{
	T_DELETE(root);
}
//----------------------------------------
template <typename TD,typename TK>
TreeItem_KA<TD,TK>* BinaryTree_KA<TD,TK>::insert(const TK& Key, const TD& value, unsigned int FR_flag)
{

	TreeItem_KA<TD,TK> **Curr,*P;
	Curr=&find(Key,&P);
	if((*Curr!=nil))
	{	
		switch(FR_flag & FRF_DOIFEXIST)
		{
		case FRF_REPLACE:
			(*Curr)->data=value;
			return *Curr;
		case FRF_ADDBEFORE:
			while(((*Curr)->left!=nil)&&((*Curr)->left->key==Key))
				Curr=&(*Curr)->left;
			if((*Curr)->left!=nil)
			{
				P=(*Curr)->left;
				(*Curr)->left=new TreeItem_KA<TD,TK>(Key,value,*Curr);
				(*Curr)->left->left=P;
				if(P)P->p=(*Curr)->left;
				return (*Curr)->left;
			}
			(*Curr)->left=new TreeItem_KA<TD,TK>(Key,value,*Curr);
			return (*Curr)->left;
		case FRF_ADDAFTER:
			while(((*Curr)->right!=nil)&&((*Curr)->right->key==Key))
				Curr=&(*Curr)->right;
			if((*Curr)->right!=nil)
			{
				P=(*Curr)->right;
				(*Curr)->right=new TreeItem_KA<TD,TK>(Key,value,*Curr);
				(*Curr)->right->right=P;
				if(P)P->p=(*Curr)->right;
				return (*Curr)->right;
			}
			(*Curr)->right=new TreeItem_KA<TD,TK>(Key,value,*Curr);
			return (*Curr)->right;
		}
		return *Curr;
	}
	if(FR_flag & FRF_ONLYEXISTING)return NULL;
	(*Curr)=new TreeItem_KA<TD,TK>(Key,value,P);
	ne++;
	return *Curr;
}
//----------------------------------------
template <typename TD,typename TK>
TreeItem_KA<TD,TK>* BinaryTree_KA<TD,TK>::delete_(const TK& Key)
{
	int R=-1;
	TreeItem_KA<TD,TK> *Y,*Z,*X,*P;
	Z=find(Key,&P);
	if(Z==nil)return NULL;
	
	if((Z->right==nil)||(Z->left==nil))Y=Z;
	else Y=successor(Z);
	
	if(Y->left!=nil) X = Y->left;
	else  X = Y->right;
	
	if((X->p = Y->p) == nil) root = X;
	else if(Y==Y->p->left) Y->p->left = X;
	else Y->p->right = X;
	if(Y!=Z)
	{
		Z->key = Y->key;
		Z->data = Y->data;
	}
	nil->p=NULL;
	Y->left = Y->right = Y->p = nil;
	ne--;
	return Y;
}
//----------------------------------------
template <typename TD,typename TK>
TD* BinaryTree_KA<TD,TK>::operator[](const TK& Key)
{
	TreeItem_KA<TD,TK>* TI=find(Key);
	if(TI!=nil)return &(TI->data);
	else return NULL;
}
//----------------------------------------
/*template <typename TD,typename TK>
int BinaryTree_KA<TD,TK>::print(int (*printTK)(const TK&), int (*printTD)(const TD&))const
{
	if(root!=nil)
		return (root->print(printTK,printTD)+printf("\n"));
	return printf("empty\n");
}*/
///===============================//
///=======CLASS RB_Tree ===========//
//-----TK must be a numeric type---------------//
//----------------------------------------
template <typename TD,typename TK>
inline RB_item<TD,TK>*& RB_Tree<TD,TK>::Find(TK Key, RB_item<TD,TK>** P)
{
	return find(Key,P);
}

//____________________________________________
//--------------------------------------------
template <typename TD,typename TK>
RB_Tree<TD,TK>::~RB_Tree()
{
	T_DELETE(root);
}

//----------------------------------------
template <typename TD,typename TK>
RB_Tree<TD,TK>::RB_Tree()
{
	nil=root=RB_item<TD,TK>::nil;
	ne=0;
}

//----------------------------------------
template <typename TD,typename TK>
RB_Tree<TD,TK>::RB_Tree(const RB_Tree<TD,TK> &Tree)
{
	nil=RB_item<TD,TK>::nil;
/*	if(!CopyDelKey)
	{
		root=nil;
		return;
	}*/
	if(Tree.root==nil)root=nil;
	else root=new RB_item<TD,TK>(Tree.root,NULL);
	ne=Tree.ne;
}

//----------------------------------------
template <typename TD,typename TK>
void RB_Tree<TD,TK>::insert_fixup(RB_item<TD,TK> *Z)
{
	RB_item<TD,TK> *Y;
	while(Z->p->color==RBT_RED)
	{
		if((Z->p)==(Z->p->p->left))
		{
            Y=Z->p->p->right;
            if(Y->color==RBT_RED)
            {
				Z->p->color=RBT_BLACK;
				Y->color=RBT_BLACK;
				Z->p->p->color=RBT_RED;
				Z=Z->p->p;
            }
            else
            {
				if(Z==Z->p->right)
				{
					Z=Z->p;
					left_rotate(Z);
				}
				Z->p->color=RBT_BLACK;
				Z->p->p->color=RBT_RED;
				right_rotate(Z->p->p);
			}
		}
		else
		{
			Y=Z->p->p->left;
			if(Y->color==RBT_RED)
			{
				Z->p->color=RBT_BLACK;
				Y->color=RBT_BLACK;
				Z->p->p->color=RBT_RED;
				Z=Z->p->p;
			}
			else
			{
				if(Z==Z->p->left)
				{
					Z=Z->p;
					right_rotate(Z);
				}
				Z->p->color=RBT_BLACK;
				Z->p->p->color=RBT_RED;
				left_rotate(Z->p->p);
			}
		}
	}
	root->color=RBT_BLACK;
}
//----------------------------------------
template <typename TD,typename TK>
int RB_Tree<TD,TK>::insert(const TK& Key, const TD* value, unsigned int FR_flag)
{
	RB_item<TD,TK> **Curr,*P;
	Curr=&find(Key,&P);
	if((*Curr!=nil))
	{
		switch(FR_flag & FRF_DOIFEXIST)
		{
		case FRF_DONOTHING:
			return -1;
		default :
			if(value)(*Curr)->data=*value;
			else return -1;
			return 1;
		}
	}
	if(FR_flag & FRF_ONLYEXISTING)return -1;
	(*Curr)=new RB_item<TD,TK>(Key, value, RBT_RED);
	(*Curr)->p=P;
	insert_fixup(*Curr);
	ne++;
	return 1;
}
//----------------------------------------
template <typename TD,typename TK>
TD* RB_Tree<TD,TK>::insert_adress(const TK& Key, const TD& value, unsigned int FR_flag)
{
	RB_item<TD,TK> **Curr,*P;
	Curr=&find(Key,&P);
	if((*Curr!=nil))
	{
		switch(FR_flag & FRF_DOIFEXIST)
		{
		case FRF_DONOTHING:
			return &((*Curr)->data);
		default :
			(*Curr)->data=value;
			return &((*Curr)->data);
		}
	}
	if(FR_flag & FRF_ONLYEXISTING)return NULL;
	(*Curr)=new RB_item<TD,TK>(Key,value,RBT_RED);
	(*Curr)->p=P;
	TD *res=&((*Curr)->data);
	insert_fixup(*Curr);
	ne++;
	return res;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item<TD,TK>& RB_Tree<TD,TK>::insert(const TK Key, const TD& value, unsigned int FR_flag)
{
	RB_item<TD,TK> **Curr,*P;
	Curr=&find(Key,&P);
	if((*Curr!=nil))
	{
		switch(FR_flag & FRF_DOIFEXIST)
		{
		case DONOTHING:
			return *nil;
		default :
			(*Curr)->data=value;
			return **Curr;
		}
	}
	if(FR_flag & FRF_ONLYEXISTING)return *nil;
	(*Curr)=new RB_item<TD,TK>(Key,value,RBT_RED);
	(*Curr)->p=P;
	insert_fixup(*Curr);
	ne++;
	return **Curr;
}
//----------------------------------------
template <typename TD,typename TK>
void RB_Tree<TD,TK>::delete_fixup(RB_item<TD,TK> *X)
{
	RB_item<TD,TK> *W;
	while((X != root)&&(X->color == RBT_BLACK))
	{
		if(X == X->p->left)
		{
			W = X->p->right;
			if(W->color == RBT_RED)
			{
				W->color = RBT_BLACK;
				X->p->color = RBT_RED;
				left_rotate(X->p);
				W = X->p->right;
			}
			if((W->left->color == RBT_BLACK)&&(W->right->color == RBT_BLACK))
			{
				W->color = RBT_RED;
				X = X->p;
			}
			else
			{
				if(W->right->color == RBT_BLACK)
				{
					W->left->color = RBT_BLACK;
					W->color = RBT_RED;
					right_rotate(W);
					W = X->p->right;
				}
				W->color = X->p->color;
				X->p->color = RBT_BLACK;
				W->right->color = RBT_BLACK;
				left_rotate(X->p);
				X = root;
			}
		}
		else
		{
			W = X->p->left;
			if(W->color == RBT_RED)
			{
				W->color = RBT_BLACK;
				X->p->color = RBT_RED;
				right_rotate(X->p);
				W = X->p->left;
			}
			if((W->left->color == RBT_BLACK)&&(W->right->color == RBT_BLACK))
			{
				W->color = RBT_RED;
				X = X->p;
			}
			else
			{
				if(W->left->color == RBT_BLACK)
				{
					W->right->color = RBT_BLACK;
					W->color = RBT_RED;
					left_rotate(W);
					W = X->p->left;
				}
				W->color = X->p->color;
				X->p->color = RBT_BLACK;
				W->left->color = RBT_BLACK;
				right_rotate(X->p);
				X = root;
			}
		}
	}
}
//----------------------------------------
template <typename TD,typename TK>
RB_item<TD,TK>* RB_Tree<TD,TK>::delete_(TK Key)
{
	RB_item<TD,TK> *Y,*Z,*X,*P;
	Z=find(Key,&P);
	if(Z==nil)return NULL;
	
	if((Z->right==nil)||(Z->left==nil))Y=Z;
	else Y=successor(Z);
	
	if(Y->left!=nil) X = Y->left;
	else  X = Y->right;
	
	if((X->p = Y->p) == nil) root = X;
	else if(Y==Y->p->left) Y->p->left = X;
	else Y->p->right = X;

	if(Y!=Z)
	{
		Z->key=Y->key;
		Z->data = Y->data;
	}
	if(Y->color == RBT_BLACK)
		delete_fixup(X);
	X->color=RBT_BLACK;
	nil->p=NULL;
	ne--;
	return Y;
}

//----------------------------------------
template <typename TD,typename TK>
int RB_Tree<TD,TK>::_delete(const TK& Key, int N)
{
	if(delete_(Key))return 1;
	return -1;
}

///===============================//
///======= CLASS RB_Tree_KA =======//
template <typename TD,typename TK>
inline RB_item_KA<TD,TK>*& RB_Tree_KA<TD,TK>::Find(const TK& Key, RB_item_KA<TD,TK>** P)
{
	return find(Key,P);
}

//____________________________________________
template <typename TD,typename TK>
RB_Tree_KA<TD,TK>::~RB_Tree_KA()
{
	T_DELETE(root);
}

//----------------------------------------
template <typename TD,typename TK>
RB_Tree_KA<TD,TK>::RB_Tree_KA()
{
	nil=root=RB_item_KA<TD,TK>::nil;
	ne=0;
}

//----------------------------------------
template <typename TD,typename TK>
RB_Tree_KA<TD,TK>::RB_Tree_KA(const RB_Tree_KA<TD,TK> &Tree)
{
	nil=RB_item_KA<TD,TK>::nil;
	if(Tree.root==nil)root=nil;
	else root=new RB_item_KA<TD,TK>(*Tree.root, NULL);
	ne=Tree.ne;
}

//----------------------------------------
template <typename TD,typename TK>
void RB_Tree_KA<TD,TK>::insert_fixup(RB_item_KA<TD,TK> *Z)
{
	RB_item_KA<TD,TK> *Y;
	while(Z->p->color==RBT_RED)
	{
		if((Z->p)==(Z->p->p->left))
		{
            Y=Z->p->p->right;
            if(Y->color==RBT_RED)
            {
				Z->p->color=RBT_BLACK;
				Y->color=RBT_BLACK;
				Z->p->p->color=RBT_RED;
				Z=Z->p->p;
            }
            else
            {
				if(Z==Z->p->right)
				{
					Z=Z->p;
					left_rotate(Z);
				}
				Z->p->color=RBT_BLACK;
				Z->p->p->color=RBT_RED;
				right_rotate(Z->p->p);
			}
		}
		else
		{
			Y=Z->p->p->left;
			if(Y->color==RBT_RED)
			{
				Z->p->color=RBT_BLACK;
				Y->color=RBT_BLACK;
				Z->p->p->color=RBT_RED;
				Z=Z->p->p;
			}
			else
			{
				if(Z==Z->p->left)
				{
					Z=Z->p;
					right_rotate(Z);
				}
				Z->p->color=RBT_BLACK;
				Z->p->p->color=RBT_RED;
				left_rotate(Z->p->p);
			}
		}
	}
	root->color=RBT_BLACK;
}
//----------------------------------------
template <typename TD,typename TK>
int RB_Tree_KA<TD,TK>::insert(const TK& Key, const TD* value, unsigned int FR_flag)
{
	RB_item_KA<TD,TK> **Curr,*P;
	Curr=&find(Key,&P);
	if((*Curr!=nil))
	{
		switch(FR_flag & FRF_DOIFEXIST)
		{
		case FRF_DONOTHING:
			return -1;
		default :
			if(value)(*Curr)->data=*value;
			else return -1;
			return 1;
		}
	}
	if(FR_flag & FRF_ONLYEXISTING)return -1;
	(*Curr)=new RB_item_KA<TD,TK>(Key, value, RBT_RED);
	(*Curr)->p=P;
	insert_fixup(*Curr);
	ne++;
	return 1;
}
//----------------------------------------
template <typename TD,typename TK>
TD* RB_Tree_KA<TD,TK>::insert_adress(const TK& Key, const TD& value, unsigned int FR_flag)
{
	RB_item_KA<TD,TK> **Curr,*P;
	Curr=&find(Key,&P);
	if((*Curr!=nil))
	{
		switch(FR_flag & FRF_DOIFEXIST)
		{
		case FRF_DONOTHING:
			return &((*Curr)->data);
		default :
			(*Curr)->data=value;
			return &((*Curr)->data);
		}
	}
	if(FR_flag & FRF_ONLYEXISTING)return NULL;
	(*Curr)=new RB_item_KA<TD,TK>(Key, value, RBT_RED);
	(*Curr)->p=P;
	TD *res=&((*Curr)->data);
	insert_fixup(*Curr);
	ne++;
	return res;
}
//----------------------------------------
template <typename TD,typename TK>
TD* RB_Tree_KA<TD,TK>::insert_adress(const TK& Key, unsigned int FR_flag)
{
	RB_item_KA<TD,TK> **Curr,*P;
	Curr=&find(Key,&P);
	if((*Curr!=nil))
	{
		switch(FR_flag & FRF_DOIFEXIST)
		{
		case FRF_DONOTHING:
			return &((*Curr)->data);
		default :
//			(*Curr)->data=value;
			return &((*Curr)->data);
		}
	}
	if(FR_flag & FRF_ONLYEXISTING)return NULL;
	(*Curr)=new RB_item_KA<TD,TK>(Key, NULL, RBT_RED);
	(*Curr)->p=P;
	TD *res=&((*Curr)->data);
	insert_fixup(*Curr);
	ne++;
	return res;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item_KA<TD,TK>& RB_Tree_KA<TD,TK>::insert(const TK& Key, const TD& value, unsigned int FR_flag)
{
	RB_item_KA<TD,TK> **Curr,*P;
	Curr=&find(Key,&P);
	if((*Curr!=nil))
	{
		switch(FR_flag & FRF_DOIFEXIST)
		{
		case FRF_DONOTHING:
			return *nil;
		default :
			(*Curr)->data=value;
			return **Curr;
		}
	}
	if(FR_flag & FRF_ONLYEXISTING)return *nil;
	(*Curr)=new RB_item_KA<TD,TK>(Key,value,RBT_RED);
	(*Curr)->p=P;
	insert_fixup(*Curr);
	ne++;
	return **Curr;
}
//----------------------------------------
template <typename TD,typename TK>
void RB_Tree_KA<TD,TK>::delete_fixup(RB_item_KA<TD,TK> *X)
{
	RB_item_KA<TD,TK> *W;
	while((X != root)&&(X->color == RBT_BLACK))
	{
		if(X == X->p->left)
		{
			W = X->p->right;
			if(W->color == RBT_RED)
			{
				W->color = RBT_BLACK;
				X->p->color = RBT_RED;
				left_rotate(X->p);
				W = X->p->right;
			}
			if((W->left->color == RBT_BLACK)&&(W->right->color == RBT_BLACK))
			{
				W->color = RBT_RED;
				X = X->p;
			}
			else
			{
				if(W->right->color == RBT_BLACK)
				{
					W->left->color = RBT_BLACK;
					W->color = RBT_RED;
					right_rotate(W);
					W = X->p->right;
				}
				W->color = X->p->color;
				X->p->color = RBT_BLACK;
				W->right->color = RBT_BLACK;
				left_rotate(X->p);
				X = root;
			}
		}
		else
		{
			W = X->p->left;
			if(W->color == RBT_RED)
			{
				W->color = RBT_BLACK;
				X->p->color = RBT_RED;
				right_rotate(X->p);
				W = X->p->left;
			}
			if((W->left->color == RBT_BLACK)&&(W->right->color == RBT_BLACK))
			{
				W->color = RBT_RED;
				X = X->p;
			}
			else
			{
				if(W->left->color == RBT_BLACK)
				{
					W->right->color = RBT_BLACK;
					W->color = RBT_RED;
					left_rotate(W);
					W = X->p->left;
				}
				W->color = X->p->color;
				X->p->color = RBT_BLACK;
				W->left->color = RBT_BLACK;
				right_rotate(X->p);
				X = root;
			}
		}
	}
}
//----------------------------------------
template <typename TD,typename TK>
TD* RB_Tree_KA<TD,TK>::operator[](const TK& Key)
{
	RB_item_KA<TD,TK>* TI=find(Key);
	if(TI!=nil)return &(TI->data);
	else return NULL;
}
//----------------------------------------
template <typename TD,typename TK>
RB_item_KA<TD,TK>* RB_Tree_KA<TD,TK>::delete_(const TK& Key)
{
	RB_item_KA<TD,TK> *Y,*Z,*X,*P;
	Z=find(Key,&P);
	if(Z==nil)return NULL;
	
	if((Z->right==nil)||(Z->left==nil))Y=Z;
	else Y=successor(Z);
	
	if(Y->left!=nil) X = Y->left;
	else  X = Y->right;
	
	if((X->p = Y->p) == nil) root = X;
	else if(Y==Y->p->left) Y->p->left = X;
	else Y->p->right = X;

	if(Y!=Z)
	{
		Z->key=Y->key;
		Z->data = Y->data;
	}
	if(Y->color == RBT_BLACK)
		delete_fixup(X);
	X->color=RBT_BLACK;
	nil->p=NULL;
	ne--;
	return Y;
}

//----------------------------------------
template <typename TD,typename TK>
int RB_Tree_KA<TD,TK>::_delete(const TK& Key, int N)
{
	if(delete_(Key))return 1;
	return -1;
}

///==================================//
#undef T_DELETE
#undef CMP_NUM
//==================================//
#endif //HASH_T_H
