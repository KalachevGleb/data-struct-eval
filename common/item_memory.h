#ifndef ITEM_MEMORY_CPP
#define ITEM_MEMORY_CPP

#define MEMORY_DEFSIZE 1000
#define MEMORY_MINSIZE 2
#define MEMORY_MAXSIZE 10000000

#define CHECK_BORDERS(s) ((s) > MEMORY_MAXSIZE ? MEMORY_MAXSIZE :((s) < MEMORY_MINSIZE ? MEMORY_MINSIZE : (s)))

#include <stdlib.h>

template <class item> class item_memory;
//---------------------------------------
template <class item>
class memory_block
{
private:
	friend class item_memory<item>;
	item *M;
	memory_block *next;
	int size;
	item* init(int Size)
	{
		if((M)||(Size<=0))return NULL;
		M=new item[size=Size];
		for(int i=1; i<size; i++)
			M[i-1].next = M+i;
		M[size-1].next=NULL;
		return M;
	}
public:
	memory_block()
	{
		size=0;
		next=NULL;
		M=NULL;
	}
	memory_block(int Size)
	{
		if(Size>0)
		{
			size=Size;
			M=new item[size];
			for(int i=1; i<size; i++)
				M[i-1].next = M+i;
			M[size-1].next=NULL;
		}
		else {M=NULL; size=0;}
		next=NULL;
	}
	~memory_block()
	{
		memory_block<item> *x, *y;
		for(int i=0; i<size; i++)M[i].next=NULL;
		delete[] M;
		M = NULL;
		for(x=next; x; x=y)
		{
			y=x->next;
			x->next=NULL;
			delete x;
		}
		next = NULL;
	}
};
/////////////////////////////////////////
//---------------------------------------
template <class item>
class mem_ptr
{
private:
	memory_block<item> *Mfirst, *Mlast;
	friend class item_memory<item>;
public:
	mem_ptr() : Mfirst(NULL),Mlast(NULL) {}
	mem_ptr<item>& operator = (mem_ptr<item> &M)
	{
		delete Mfirst;
		Mfirst=M.Mfirst;
		Mlast=M.Mlast;
		M.Mfirst=M.Mlast=NULL;
	}
	~mem_ptr()
	{
		delete Mfirst;
		Mfirst=Mlast=NULL;
	}
};

template <class item>
class item_memory
{
protected:
	static int def_size;
	bool if_reinit;
	mem_ptr<item> M;
	item *free;
	int size;
	int free_size;
public:
	item_memory() : free(NULL), if_reinit(true), M(), size(def_size), free_size(0){}
	~item_memory(){}
	int get_size();
	int set_size(int Size);
	int get_free_size();
	static int get_def_size();
	static int set_def_size(int);
	item* new_item();
	void delete_item(item*&);
	int defragment();
};
//---------------------------------------
template <class item>
int item_memory<item>::def_size=MEMORY_DEFSIZE;
//---------------------------------------
template <class item>
inline int item_memory<item>::get_def_size()
{
	return def_size;
}
//---------------------------------------
template <class item>
inline int item_memory<item>::set_def_size(int Size)
{
	if(size>=1)def_size=Size;
	return def_size;
}
//---------------------------------------
template <class item>
inline int item_memory<item>::get_free_size()
{
	return free_size;
}
//---------------------------------------
template <class item>
inline int item_memory<item>::get_size()
{
	return size;
}
//---------------------------------------
template <class item>
inline int item_memory<item>::set_size(int Size)
{
	return (size=CHECK_BORDERS(Size));
}
//---------------------------------------
template <class item>
item* item_memory<item>::new_item()
{
	item *res;
	if(free==NULL)
	{
		if(!M.Mlast)M.Mfirst = M.Mlast = new memory_block<item>();
		else M.Mlast = M.Mlast->next = new memory_block<item>();
		free = M.Mlast->init(size);
		free_size+=size;
	}
	res=free;
	free=free->next;
	res->next=NULL;
	free_size--;
	return res;
}
//---------------------------------------
template <class item>
void item_memory<item>::delete_item(item *&x)
{
	if(x==NULL)return;
	if(if_reinit)*x = item();
	x->next=free;
	free = x;
	x=NULL;
	free_size++;
}
//---------------------------------------
template <class item>
class item_memory_s
{
protected:
	static bool if_reinit;
	static mem_ptr<item> M;
	static item *free;
	static int size;
	static int free_size;
public:
	static int get_size();
	static int set_size(int Size);
	static int get_free_size();
	static item* new_item();
	static void delete_item(item*&);
	static int defragment();
};
//---------------------------------------
template <class item>
item *item_memory_s<item>::free = NULL;
//---------------------------------------
template <class item>
bool item_memory_s<item>::if_reinit = true;
//---------------------------------------
template <class item>
mem_ptr<item> item_memory_s<item>::M;
//---------------------------------------
template <class item>
int item_memory_s<item>::size = MEMORY_DEFSIZE;
//---------------------------------------
template <class item>
int item_memory_s<item>::free_size = 0;
//---------------------------------------
template <class item>
inline int item_memory_s<item>::get_free_size()
{
	return free_size;
}
//---------------------------------------
template <class item>
inline int item_memory_s<item>::get_size()
{
	return size;
}
//---------------------------------------
template <class item>
inline int item_memory_s<item>::set_size(int Size)
{
	return (size=CHECK_BORDERS(Size));
}
//---------------------------------------
template <class item>
item* item_memory_s<item>::new_item()
{
	item *res;
	if(free==NULL)
	{
		if(!M.Mlast)M.Mfirst = M.Mlast = new memory_block<item>();
		else M.Mlast = M.Mlast->next = new memory_block<item>();
		free = M.Mlast->init(size);
		free_size+=size;
	}
	res=free;
	free=free->next;
	res->next=NULL;
	free_size--;
	return res;
}
//---------------------------------------
template <class item>
void item_memory_s<item>::delete_item(item *&x)
{
	if(x==NULL)return;
	if(if_reinit)*x = item();
	x->next=free;
	free = x;
	x=NULL;
	free_size++;
}
//---------------------------------------
template <class T>
class def_allocator
{
public:
	T *New(const T &data)
	{
		return new T(data);
	}
	void Delete(T* ptr)
	{
		delete ptr;
	}
};
//---------------------------------------
#endif //ITEM_MEMORY_CPP
