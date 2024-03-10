#include "pst.h"
#include <stdio.h>

inline int Min(int x, int y){return x<y ? x : y;}
inline int Max(int x, int y){return x>y ? x : y;}
inline int Abs(int x){return x>=0 ? x : -x;}

PSTBase::PSTBase(){
	nil.l = nil.r = nil.p = &nil;
	nil.c = BLACK;
	nil.duplQ = false;
	nil.is_nil = true;
	nil.q.x = nil.q.y = (int)((unsigned int)(-1)>>1);//maximum integer
	sz=0;
}

PSTBase::~PSTBase(){}

void PSTBase::print(PSTBase::PSTNodeBase *n, int rec, bool sh){
	if(n->is_nil)return;
	//__builtin_ia32_comisdeq();
	print(n->l, rec+1, sh);
	for(int i=0; i<rec; i++)printf("  ");
	if(sh)printf("%s, (%d, %d)\n", n->c == RED ? "RED" : "BLACK", n->q.y, n->q.x);
	else{
		if(n->p!=&nil)printf("P(%d, %d)", n->p->q.y, n->p->q.x);
		else printf("P = 0  ");
		for(int i=rec; i<10; i++)printf("  ");
		printf("| %s |", n->duplQ ? "duplQ" : "     ");
		printf(" Q(%d, %d)\n", n->q.y, n->q.x);
	}
	print(n->r, rec+1, sh);
}

int PSTBase::check(PSTBase::PSTNodeBase *n){
	if(n->is_nil)return 0;
	int dl = check(n->l), dr = check(n->r);
	bool ret = (dl>=0 && dr>=0);
	if(Abs(dl)!=Abs(dr)){printf("black disbalance in (%d,%d)\n", n->q.x, n->q.y);ret = false;}
	int r = Max(Abs(dl),Abs(dr));
	if(n->c==BLACK){r++;}
	else{
		if(n->l->c == RED || n->r->c == RED){
			ret = false;
			printf("two red nodes in (%d,%d)\n", n->q.x, n->q.y);
		}
	}
	if(!n->duplQ && n->p && (n->p->q.y > n->q.y)){
		ret = false;
		printf("p.y > q.y in (%d,%d)\n", n->q.x, n->q.y);
	}
	return ret ? r : -r;
}
//int PSTBase::checkPST(PSTBase::PSTNodeBase *n){
//	if(n->is_nil)return
//}

const PSTBase::PSTNodeBase *PSTBase::_find(const PSTBase::Point &pt) const{
	const PSTNodeBase *n;
	for(n = nil.l; !n->is_nil; ){
		int r = pt.cmp(n->q);
		if(r<0)n=n->l;
		else if(r>0)n=n->r;
		else return n;
	}
	return &nil;
}

bool PSTBase::contains(int x, int y)const{
	return _contains(Point(x,y));
}


PSTBase::PSTNodeBase* PSTBase::__insert(/*PSTBase::PSTNodeBase *&t, */ Point pt){
	const int max_depth = 100; //for <= 2^50 nodes
	PSTNodeBase *pptr, *A=0, *B, *n;
	ptr_t *ptr = &nil.l, *st[max_depth], _nil = &nil;
	st[0] = &_nil;
	int it = 1, bn, y = pt.y;
	while(!(*ptr)->is_nil){
		st[it++] = ptr;
		pptr = *ptr;
		if(!A && y < pptr->p->q.y)
			A = pptr;

		int cmp = pt.cmp(pptr->q);
		if(!cmp)return pptr;
		if(cmp<0) ptr = &pptr->l;
		else      ptr = &pptr->r;
	}
	st[it] = ptr;

	n = _insnode(pt.x,pt.y); sz++;
	if(A){
		disposeP(A);
		A->p = n;
		n->p = &nil;
	}else n->p = n;

	n->l = n->r = &nil;
	n->duplQ = true;
	*ptr = n;
	if(it>1){
		n->c = RED;
		if((*st[it-1])->c == BLACK)return n;
	}else{
		n->c = BLACK;
		return n;
	}
	for(bn = it-2; bn>0; bn--){
		pptr = *st[bn];
		if(pptr->c == BLACK){
			if(pptr->l->c == BLACK || pptr->r->c == BLACK)break;
			pptr->l->c = pptr->r->c = BLACK;
			pptr->c = RED;
			if((*st[--bn])->c == BLACK)return n;
		}
	}
	if(!bn)return n;
	A = *st[bn+1]; B = *st[bn+2];
	pptr->c = RED;
	if(A == pptr->l){
		if(B == A->r)leftlongrot(*st[bn]);
		else rightrot(*st[bn]);
	}else{
		if(B == A->l)rightlongrot(*st[bn]);
		else leftrot(*st[bn]);
	}
	(*st[bn])->c = BLACK;
	return n;
}

bool PSTBase::extractMaxQX(PSTBase::ptr_t &t, PSTNodeBase *& ep, bool &duplAsP){
	duplAsP = false;
	bool bal;
	if(t->r->is_nil){
		disposeP(t);
		duplAsP = t->duplQ;
		ep = t; t = t->l;
		if(ep->c == BLACK){
			if(t->c == RED){
				t->c = BLACK;
				return false;
			}else return true;
		}else return false;
	} else {
		bal = extractMaxQX(t->r, ep, duplAsP);
		if(duplAsP){
			if(t->p == ep){
				duplAsP = false;
				extractP(t);
			}
		}
		if(bal)return delbalance(t, t->r);
	}
	return false;
}

bool PSTBase::_remove(PSTBase::ptr_t &t, const Point &pt){
	if(t->is_nil)return false;
	PSTNodeBase *bal = 0;
	int cmp = pt.cmp(t->q);
	if(!cmp){
		_del = t;
		disposeP(t);
		if(t->l->is_nil || t->r->is_nil){
			if(t->l->is_nil)t=t->r;
			else t=t->l;
			if(_del->c == BLACK){
				if(t->c == RED){
					t->c = BLACK;
					return false;
				}else return true;
			}else return false;
		}else{
			bool dQ;
			PSTNodeBase *mq;
			if(extractMaxQX(t->l, mq, dQ))
				bal = t->l;
			mq->c = t->c; mq->p = t->p;
			mq->duplQ = dQ;
			mq->l = t->l; mq->r = t->r;
			t = mq;
			extractP(t);
		}
	}else{
		if(cmp < 0){
			if(_remove(t->l, pt))bal = t->l;
		}else if(_remove(t->r, pt))bal = t->r;

		if(t->p == _del)
			extractP(t);
	}
	if(bal)return delbalance(t, bal);
	return false;
}

void PSTBase::disposeP(PSTBase::PSTNodeBase *n){
	SETUP_COUNTER
	if(n->p->is_nil)return;
	PSTNodeBase *newp, *tmp = n->p;
	n->p = &nil;
	for(; tmp!=&nil;){
		if(tmp == n)break;
		if(tmp->q < n->q)n = n->l;
		else n = n->r;
		newp = tmp;
		tmp = n->p;
		n->p = newp;
	}
	tmp->duplQ = false;
}

void PSTBase::extractP(PSTBase::PSTNodeBase *n){
	SETUP_COUNTER
	while(n!=&nil){
		int lY = n->l->p->q.y,
			rY = n->r->p->q.y;
		if(!n->duplQ && n->q.y <= Min(lY,rY))break;
		if(lY < rY){
			n->p = n->l->p;
			n = n->l;
		}else{
			n->p = n->r->p;
			if(n->p->is_nil)return;
			n = n->r;
		}
	}
	n->p = n;
	n->duplQ = true;
}

bool PSTBase::_enumerate(PSTBase::PSTNodeBase * n, int l, int r, int b, void *obj, bool push(void*, PSTBase::PSTNodeBase*)){
	SETUP_COUNTER
	if(n->p->q.y > b)return true;
	if(!n->duplQ && n->q.inRange(l,r,b))
		if(!push(obj, n))return false;
	if(n->p->q.inRange(l,r,b))
		if(!push(obj, n->p))return false;
	if(l <= n->q.x)
		if(!_enumerate(n->l, l, r, b, obj, push))return false;
	if(r >= n->q.x)
		if(!_enumerate(n->r, l, r, b, obj, push))return false;
	return true;
}

PSTBase::PSTNodeBase *PSTBase::_minY(int l, int r, int b, PSTBase::PSTNodeBase *n){
	if(n->is_nil)return &nil;
	int ymin = n->q.y;
	PSTNodeBase *res = &nil;
	if(n->q.inRange(l,r,b))res = n;
	if(n->p){
		if(n->p->q.inRange(l,r,b)){
			if(res->q.y < n->p->q.y) return res;
			else return n->p;
		}
		ymin = Min(ymin, n->p->q.y);
	}else return res;
	if(ymin>b)return &nil;
	PSTNodeBase *L=&nil, *R=&nil;
	if(l <= n->q.x){
		L = _minY(l,r,b,n->l);
		if(res->q.y > L->q.y)res = L;
	}
	if(r > n->q.x){
		R = _minY(l,r,b,n->r);
		if(res->q.y > R->q.y)res = R;
	}
	return res;
}

void PSTBase::rightrot(PSTBase::ptr_t &t){
	PSTNodeBase *e = t, *c = e->l;
	t = c;
	disposeP(e);
	disposeP(c);
	e->l = c->r; c->r = e;
	extractP(e);
	extractP(c);
}
void PSTBase::leftrot(PSTBase::ptr_t &t){
	PSTNodeBase *e = t, *c = e->r;
	t = c;
	disposeP(e);
	disposeP(c);
	e->r = c->l; c->l = e;
	extractP(e);
	extractP(c);
}
void PSTBase::leftlongrot(PSTBase::ptr_t &t){
	PSTNodeBase *C = t, *A = C->l, *B = A->r;
	t = B;
	disposeP(C);
	disposeP(A);
	disposeP(B);
	A->r = B->l; B->l = A;
	C->l = B->r; B->r = C;
	extractP(C);
	extractP(A);
	extractP(B);
}
void PSTBase::rightlongrot(PSTBase::ptr_t &t){
	PSTNodeBase *C = t, *A = C->r, *B = A->l;
	t = B;
	disposeP(C);
	disposeP(A);
	disposeP(B);
	A->l = B->r; B->r = A;
	C->r = B->l; B->l = C;
	extractP(C);
	extractP(A);
	extractP(B);
}
bool PSTBase::insbalance(PSTBase::ptr_t &p, PSTBase::PSTNodeBase *l){
	if(p->c == RED)return (l->c == RED);
	p->c = RED;
	if(p->l->c == p->r->c){//then p->l->c = p->r->c = l->c = RED
		p->l->c = p->r->c = BLACK;
	}else{
		if(l == p->l){
			if(l->r->c == RED)leftlongrot(p);
			else rightrot(p);
		}else{//l == p->r
			if(l->l->c == RED)rightlongrot(p);
			else leftrot(p);
		}
		p->c = BLACK;
	}
	return (p->c == RED);
}
bool PSTBase::delbalance(ptr_t &p, PSTBase::PSTNodeBase *x){
	if(x->c==RED){x->c = BLACK; return false;}
	if(x == p->l){
		PSTNodeBase *D = p->r, *C = D->l;
		if(D->c == RED){ //then p is black and C is black
			if(C->r->c==BLACK){
				if(C->l->c==BLACK){
					leftrot(p);
					C->c = RED; D->c = BLACK;
					return false;
				}else{
					PSTNodeBase *A = C->l;
					disposeP(p);         /*          [P]                      [D]      */
					disposeP(D);         /*         /   \                    /   \     */
					disposeP(C);         /*      [[x]]  (D)                (A)    v    */
					disposeP(A);         /*             / \               /   \        */
					p->r = A->l;         /*           [C]  v   --->     [P]   [C]      */
					C->l = A->r;         /*           / \               / \   / \      */
					A->l = p; A->r = C;  /*         (A)  u            [x]  s t   u     */
					D->l = A;            /*         / \                                */
					extractP(p);         /*        s   t                               */
					extractP(C);         /*                                            */
					extractP(A);         /*   [...] -- BLACK node, (...) -- RED node,  */
					extractP(D);         /*   s,t,u,v -- any subtrees                  */
					p = D; p->c = BLACK;
					return false;
				}
			}else{ //C->r is RED
				disposeP(p);             /*         [P]                     [D]        */
				disposeP(D);             /*        /   \                   /   \       */
				disposeP(C);             /*     [[x]]  (D)               (C)    t      */
				p->r = C->l;             /*            / \    --->      /   \          */
				C->l = p; C->c = RED;    /*          [C]  t           [P]   [B]        */
				D->l = C; D->c = BLACK;  /*          / \              / \              */
				C->r->c = BLACK;         /*         s  (B)          [x]  s             */
				extractP(p);             /*                                            */
				extractP(C);             /*   [...] -- BLACK node, (...) -- RED node,  */
				extractP(D);             /*   s,t -- any subtrees                      */
				p = D; return false;
			}
		}else{ //D is BLACK
			PSTNodeBase *E = D->r;
			if(E->c == BLACK){
				if(C->c == BLACK){
					D->c = RED;
					if(p->c == BLACK)return true; //when returns true, no rotations done
					p->c = BLACK;
					return false;
				}else{
					C->c = p->c;
					p->c = BLACK;
					rightlongrot(p);
					return false;
				}
			}else{//E is RED
				D->c = p->c;
				p->c = BLACK;
				E->c = BLACK;
				leftrot(p);
				return false;
			}
		}
	}else{
		PSTNodeBase *D = p->l, *C = D->r;
		if(D->c == RED){ //then p is black and C is black
			if(C->l->c==BLACK){
				if(C->r->c==BLACK){
					rightrot(p);
					C->c = RED; D->c = BLACK;
					return false;
				}else{
					PSTNodeBase *A = C->r;
					disposeP(p);         /*          [P]                [D]            */
					disposeP(D);         /*         /   \              /   \           */
					disposeP(C);         /*       (D)  [[x]]          s    (A)         */
					disposeP(A);         /*       / \                     /   \        */
					p->l = A->r;         /*      s  [C]        --->     [C]   [P]      */
					C->r = A->l;         /*         / \                 / \   / \      */
					A->r = p; A->l = C;  /*        t  (A)              t   u v  [x]    */
					D->r = A;            /*           / \                              */
					extractP(p);         /*          u   v                             */
					extractP(C);         /*                                            */
					extractP(A);         /*   [...] -- BLACK node, (...) -- RED node,  */
					extractP(D);         /*   s,t,u,v -- any subtrees                  */
					p = D; p->c = BLACK;
					return false;
				}
			}else{ //C->r is RED
				disposeP(p);             /*         [P]               [D]              */
				disposeP(D);             /*        /   \             /   \             */
				disposeP(C);             /*      (D)  [[x]]         s    (C)           */
				p->l = C->r;             /*      / \          --->      /   \          */
				C->r = p; C->c = RED;    /*     s  [C]                [B]   [P]        */
				D->r = C; D->c = BLACK;  /*        / \                      / \        */
				C->l->c = BLACK;         /*      (B)  t                    t  [x]      */
				extractP(p);             /*                                            */
				extractP(C);             /*   [...] -- BLACK node, (...) -- RED node,  */
				extractP(D);             /*   s,t -- any subtrees                      */
				p = D; return false;
			}
		}else{ //D is BLACK
			PSTNodeBase *E = D->l;
			if(E->c == BLACK){
				if(C->c == BLACK){
					D->c = RED;
					if(p->c == BLACK)return true; //when returns true, no rotations done
					p->c = BLACK;
					return false;
				}else{
					C->c = p->c;
					p->c = BLACK;
					leftlongrot(p);
					return false;
				}
			}else{//E is RED
				D->c = p->c;
				p->c = BLACK;
				E->c = BLACK;
				rightrot(p);
				return false;
			}
		}
	}
}
