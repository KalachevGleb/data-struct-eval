#include "stringmap.h"
#include <cstring>
/** \class StringMap
  *\brief Множество со строкой в качестве ключа.
  */

template<class T> T Max(T x, T y){return x>y ? x : y;}
template<class T> T Min(T x, T y){return x<y ? x : y;}

StringTreeBase::~StringTreeBase(){}

StringTreeBase::_Node *StringTreeBase::_find0(const char *str){
	_Node *n = _nil->l, *ln = _nil;
	uint m=0;
	while(n!=_nil){
		for(;str[m] == n->key[m];m++)
			if(!str[m])return n; //in this case str=n->key;

		if(str[m] < n->key[m]){
			if(n->prev->sr < m){_nil->r=n; return _nil;}
			ln = n;  n=n->l;
			while(n->sr<m)n=n->r;
		}else{
			if(n->next->sl < m){_nil->r=n->next; return _nil;}
			ln = n->next; n=n->r;
			while(n->sl<m)n=n->l;
		}
	}
	_nil->r = ln;
	return _nil;
}

StringTreeBase::_Node *StringTreeBase::_find(const char *str)const{
	_Node *n = _nil->l;
	for(uint m=0; n->h; ){
		for(;str[m] == n->key[m];m++)
			if(!str[m])return (_Node*)n; //in this case str=n->key;
		if(str[m] < n->key[m])
			for(n=n->l; n->sr<m;)n=n->r;
		else
			for(n=n->r;n->sl<m;)n=n->l;
	}
	return (_Node*)_nil;
}

uint32_t commonSymbols(const char* a, const char *b, uint32_t l){
	uint32_t i;
	for(i=l; a[i]==b[i];)i++;
	return i;
}

void StringTreeBase::_insert(StringTreeBase::_Node *before, StringTreeBase::_Node *n){
	_sz++;
	_Node *bp = before->prev;
	if(before->l->h){
		n->p = bp;
		bp->r = n;
		n->sr = commonSymbols(before->key, n->key, bp->sr);
		n->sl = commonSymbols(bp->key, n->key, bp->sr);
	}else{
		n->p = before;
		before->l = n;
		if(before->h){
			n->sr = commonSymbols(n->key, before->key, before->sl);
			n->sl = commonSymbols(n->key, bp->key, before->sl);
		}else n->sr = n->sl = 0;
		//then before = nil and root = nil->l=nil => tree is empty
	}
	n->l = n->r = _nil;
	n->next = before; n->prev = bp;
	before->prev = n; bp->next = n;
	n->h = 1;
	_balance(n->p);
}

void StringTreeBase::_delete(StringTreeBase::_Node *n){
	_sz--;
	if(n->r->h){
		_Node *d = n->next, *x;
		if(n->l->h){
			uint ds = d->sl;
			for(x = n->prev; x!=n; x = x->p){
				if(x->sr > ds)x->sr = d->sl;
				else break;
			}
		}
		if(d->sl > n->sl)d->sl = n->sl;
		(d->prev = n->prev)->next = d;
		if(n->r->l->h){
			_Node *p = d->p;            
			p->sl = d->sr; p->l = d->r; d->r->p = p;
			for(x = p->p; x!=n; x = x->p){
				x->sl = Min(x->l->sl, x->l->sr);
			//	if(x->sl >= x->sr)break;
			}
			d->sr = Min(n->r->sl, n->r->sr); /*     n___              d___      */
			d->p = n->p; d->h = n->h;        /*    /    \            /    \     */ 
			(d->l = n->l)->p = d;            /*   a     /\          a     /\    */
			(d->r = n->r)->p = d;            /*        p...   -->        p...   */
			if(n->p->l==n)n->p->l = d;       /*       / \..\            / \..\  */
			else n->p->r = d;                /*      d   ...           b   ...  */
			                                 /*       \                         */
			_balance(p);                     /*        b                        */
		}else{
			d->h = n->h;                       /*     n                     */
			d->p = n->p; (d->l = n->l)->p = d; /*    / \              d     */
			if(n->p->l==n)n->p->l = d;         /*   a   d     -->    / \    */
			else n->p->r = d;                  /*        \          a   c   */
			_balance(d);                       /*         c                 */
		}
	}else{
		uint s = n->sr;
		if(n->l->h)
			for(_Node *x = n->prev; x!=n; x = x->p){
				if(x->sr > s)x->sr = s;
				else break;
			}
		_Node *p = n->p;
		n->next->prev = n->prev;
		n->prev->next = n->next;
		if(p->l == n) p->l = n->l;
		else p->r = n->l;
		n->l->p = p;
		_balance(p);
	}
}

//AVL balancing with correcting fields sl ans sr of changed nodes
void StringTreeBase::_balance(StringTreeBase::_Node *x){
	for(;x->h; x = x->p){
		auto px = x->h;
		if(x->l->h == x->r->h+2){ //
			_Node *d = x->l, *c = d->r, *e = d->l;// *a = x->r;
			if(c->h <= e->h){
				if(x->p->l == x)x->p->l = d;               /*     x             d       */
				else x->p->r = d;                          /*    / \           / \      */
				d->r = x;   d->p = x->p;   x->p = d;       /*   d   a   -->   e   x     */
				x->l = c;   c->p = x;                      /*  / \               / \    */
				x->h = c->h+1;             d->h = x->h+1;  /* e   c             c   a   */

				x->sl = d->sr;
				if(d->sr > x->sr)d->sr = x->sr;
				x = d;
			}else{//c->h > e->h, x->h = d->h+1 = c->h+2 = e->h+3;
				_Node *cl = c->l, *cr = c->r;    /*      x                 c          */
				if(x->p->l == x)x->p->l = c;     /*     / \              /   \        */
				else x->p->r = c;                /*    d   a    -->     d     x       */
				c->r = x; c->p = x->p; x->p = c; /*   / \              / \   / \      */
				c->l = d; d->p = c;              /*  e   c            e  cl cr  a     */
				cr->p = x; x->l = cr;            /*     / \                           */
				cl->p = d; d->r = cl;            /*    cl cr                          */

				x->h = d->h = c->h; ++c->h;

				x->sl = c->sr; d->sr = c->sl;
				if(c->sr > x->sr)c->sr = x->sr;
				if(c->sl > d->sl)c->sl = d->sl;
				x = c;
			}
		}else if(x->l->h+2 == x->r->h){//
			_Node *d = x->r, *c = d->l, *e = d->r;//*a = x->l;
			if(c->h <= e->h){
				if(x->p->l == x)x->p->l = d;              /*     x               d    */
				else x->p->r = d;                         /*    / \             / \   */
				d->l = x;   d->p = x->p;   x->p = d;      /*   a   d    -->    x   e  */
				x->r = c;   c->p = x;                     /*      / \         / \     */
				x->h = c->h+1;             d->h = x->h+1; /*     c   e       a   c    */

				x->sr = d->sl;
				if(d->sl > x->sl)d->sl = x->sl;
				x = d;
			}else{//c->h > e->h, x->h = d->h+1 = c->h+2 = e->h+3;
				_Node *cl = c->l, *cr = c->r;    /*    x                 c        */
				if(x->p->l == x)x->p->l = c;     /*   / \              /   \      */
				else x->p->r = c;                /*  a   d    -->     x     d     */
				c->l = x; c->p = x->p; x->p = c; /*     / \          / \   / \    */
				c->r = d; d->p = c;              /*    c   e        a  cl cr  e   */
				cl->p = x; x->r = cl;            /*   / \                         */
				cr->p = d; d->l = cr;            /*  cl cr                        */

				x->h = d->h = c->h; ++c->h;

				x->sr = c->sl; d->sl = c->sr;
				if(c->sl > x->sl)c->sl = x->sl;
				if(c->sr > d->sr)c->sr = d->sr;
				x = c;
			}
		}else x->h = Max(x->l->h, x->r->h)+1;
		if(px == x->h)break;
	}
}

StringTreeBase::StringTreeBase(StringTreeBase::_Node *nil){
	nil->l = nil->r = nil->p = _nil = nil;
	nil->next = nil->prev = nil;
	nil->key = nullptr;
    nil->h = 0;
	nil->sl = nil->sr = uint(-1);
	_sz = 0;
}
