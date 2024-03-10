#include "rbtree.h"
#include "commontree.h"
#include <stdio.h>

inline int Min(int x, int y){return x<y ? x : y;}
inline int Max(int x, int y){return x>y ? x : y;}
inline int Abs(int x){return x>=0 ? x : -x;}

RBTBase::RBTBase(){
	nil.setL(&nil); nil.setR(&nil); nil.setP(&nil);
	nil.setC(BLACK); nil.setNil();
	sz=0;
}

void RBTBase::print(RBTBase::RBTNodeBase *n, int rec){
	if(n->isNil())return;
	print(n->left(), rec+1);
	for(int i=0; i<rec; i++)printf("  ");
	printf("%s\n", n->c() == RED ? "RED" : "BLACK");
	print(n->right(), rec+1);
}


int RBTBase::check(RBTBase::RBTNodeBase *n){
	if(n->isNil())return 0;
	int dl = check(n->left()), dr = check(n->right());
	bool ret = (dl>=0 && dr>=0);
	if(Abs(dl)!=Abs(dr)){printf("black disbalance in \n");ret = false;}
	int r = Max(Abs(dl),Abs(dr));
	if((!n->left()->isNil() && n->left()->up()!=n) ||
		(!n->right()->isNil() && n->right()->up()!=n)){
		printf("wrong p field\n");
		ret = false;
	}
	if(n->c()==BLACK){r++;}
	else{
		if(n->left()->c() == RED || n->right()->c() == RED){
			ret = false;
			printf("two red nodes in \n");
		}
	}
	return ret ? r : -r;
}

bool RBTBase::_remove(RBTBase::RBTNodeBase *t){
	if(t->isNil())return false;
	RBTNodeBase *p, *x, *tn;
//	if(_first == t)_first = t->next();
//	if(nil.r == t)nil.r = t->prev();
	if(t->left()->isNil() || t->right()->isNil()){
		p = t->up();
		if(t->left()->isNil())tn=t->right();
		else tn=t->left();
		if(t == p->left())p->setL(tn);
		else p->setR(tn);
		if(!tn->isNil())
			tn->setP(p);
		if(t->c() == BLACK){
			if(tn->c() == RED)tn->setC(BLACK);
			else delbalance(tn, p);
		}
	}else{
		tn = t->left();
		if(tn->right()->isNil()){                /*        t       */
			x = tn->left();                      /*       /        */
			p = tn;                              /*      tn        */
			//tn->left()->c = BLACK;             /*     x  nil     */
		}else{
			do tn = tn->right();                 /*       ___t     */
			while(!tn->right()->isNil());        /*      /    \    */
			p = tn->up();                        /*     L          */
			x = p->setR(tn->left());x->setP(p);  /*     ...        */
			tn->setL(t->left());                 /*       p        */
			tn->left()->setP(tn);                /*        tn      */
		}                                        /*       x  nil   */
		if(tn->c() == RED)x=nullptr;
		else if(x->c() == RED){x->setC(BLACK); x=nullptr;}
		tn->setC(t->c()); tn->setP(t->up());
		if(t->up()->left() == t)t->up()->setL(tn);
		else t->up()->setR(tn);
		tn->setR(t->right());
		tn->right()->setP(tn);
		if(x) delbalance(x,p);
	}
	return true;
}

RBTBase::RBTNodeBase * RBTBase::rightrot(RBTBase::RBTNodeBase *x){
	RBTNodeBase *c = x->left();
	x->setL(c->right());  /*       x           c      */
	c->setR(x);           /*      / \         / \     */
	x->left()->setP(x);   /*     c   u  -->  s   x    */
	x->setP(c);           /*    / \             / \   */
	return c;             /*   s   t           t   u  */
}
RBTBase::RBTNodeBase * RBTBase::leftrot(RBTNodeBase *t){
	RBTNodeBase *c = t->right();
	t->setR(c->left());   /*     x               c    */
	c->setL(t);           /*    / \             / \   */
	t->right()->setP(t);  /*   s   c    -->    x   u  */
	t->setP(c);           /*      / \         / \     */
	return c;             /*     t   u       s   t    */
}
RBTBase::RBTNodeBase * RBTBase::leftlongrot(RBTBase::RBTNodeBase *C){
	RBTNodeBase *A = C->left(), *B = A->right();
	A->setR(B->left()); B->setL(A);  /*       C              B       */
	C->setL(B->right()); B->setR(C); /*      / \           /   \     */
	/* */                            /*     A   v         A     C    */
	A->setP(B); C->setP(B);          /*    / \     -->   / \   / \   */
	A->right()->setP(A);             /*   s   B         s   t u   v  */
	C->left()->setP(C);              /*      / \                     */
	return B;                        /*     t   u                    */
}
RBTBase::RBTNodeBase * RBTBase::rightlongrot(RBTNodeBase *A){
	RBTNodeBase *C = A->right(), *B = C->left();
	C->setL(B->right()); B->setR(C); /*     A                B       */
	A->setR(B->left()); B->setL(A);  /*    / \             /   \     */
	/* */                            /*   s   C           A     C    */
	C->setP(B); A->setP(B);          /*      / \   -->   / \   / \   */
	C->left()->setP(C);              /*     B   v       s   t u   v  */
	A->right()->setP(A);             /*    / \                       */
	return B;                        /*   t   u                      */
}

void RBTBase::insbalance(RBTBase::RBTNodeBase *x){
	//x is inserted node, so x is RED
	RBTNodeBase *P, *B, *pp;
	//if(p->c == BLACK){nil.l->c = BLACK; return;}
	for(;;x=P){ //now x is RED
		x = x->up();
		if(x->c() == BLACK){
			nil.left()->setC(BLACK);
			return;
		}
		P = x->up(); //x is RED, P is BLACK
		if(P->left() == x)B = P->right();    /*      s or t is RED      */
		else B = P->left();                  /*     [P]          (P)    */
		if(B->c() == RED){                   /*     / \          / \    */
			x->setC(BLACK); B->setC(BLACK);  /*   (x) (B) -->  [x] [B]  */
			P->setC(RED);                    /*   / \          / \      */
		}else break;                         /*  s   t        s   t     */
		//at the end of cycle P is RED;
	}
	//now P is BLACK, x is RED, B is BLACK, so may be 4 cases:
	/*             case 1                           case 2              */
	/*       [P]             [A]               [P]            [x]       */
	/*      /   \           /   \             /   \          /   \      */
	/*    (x)   [B]  --> (x)     (P)   OR   (x)   [B] -->  (s)   (P)    */
	/*    / \            / \     / \        / \                  / \    */
	/*  [s] (A)        [s] [t] [u] [B]    (s) [t]              [t] [B]  */
	/*      / \                                                         */
	/*    [t] [u]                                                       */
	/*                or 2 symmetric cases if x = P->r                  */
	pp = P->up();
	if(x == P->left()){
		if(x->right()->c() == RED)x = leftlongrot(P);  //1st case
		else x = rightrot(P);                  //2nd case
	}else{//x == P->r
		if(x->left()->c() == RED)x = rightlongrot(P); //symmetric to 1st case;
		else x = leftrot(P);                   //symmetric to 2nd case;
	}
	P->setC(RED); x->setC(BLACK);
	x->setP(pp);
	if(P == pp->left())pp->setL(x);
	else pp->setR(x);
}

void RBTBase::delbalance(RBTBase::RBTNodeBase *x, RBTBase::RBTNodeBase *pn){
	RBTNodeBase *P = pn, *pp, *A, *B, *C, *D, *E, *n0 = nil.left();
	for(;x != n0;){
		if(x->c() == RED){  /*     (P)          [P]             [P]         [[P]]      */
			x->setC(BLACK); /*    /   \   -->  /   \    OR     /   \   -->  /   \      */
			return;         /* [[x]]  [C]    [x]   (C)      [[x]]  [C]    [x]   (C)    */
		}                   /*        / \          / \             / \          / \    */
		/* */               /*      [B] [D]      [B] [D]         [B] [D]      [B] [D]  */
		if (x == P->left()) C = P->right();
		else C = P->left();
		if (C->c() || C->left()->c() || C->right()->c())break;
		C->setC(RED);
		x = P; P = x->up();
	}
	if(x==n0)return;

	//now one of D, D->l, D->r is RED;
	pp = P->up();
	B = C->left(); D = C->right();
	if(C->c() == BLACK){ //then B or D is RED
		/*     now may be 4 cases:                                       */
		/*       1                                2                      */
		/*      {P}             {B}              {P}             {C}     */
		/*     /   \           /   \            /   \           /   \    */
		/*  [[x]]  [C]  -->  [P]   [C]   OR  [[x]]  [C]  -->  [P]   [D]  */
		/*         / \       / \   / \              / \       / \        */
		/*       (B) [D]   [x]  s t  [D]           B  (D)   [x]  B       */
		/*       / \                                                     */
		/*      s   t                                                    */
		/*                   AND 2 SYMMETRIC CASES                       */
		/*   [...] -- BLACK node, (...) -- RED node, {...} -- any color  */
		/*   s,t -- any subtrees                                         */
		if(x == P->left()){
			if(D->c() == BLACK){ //then  B is RED, 1st case
				B->setC(P->c());
				x = rightlongrot(P);
			}else{  //then D is RED, 2nd case
				C->setC(P->c());
				D->setC(BLACK);
				x = leftrot(P);
			}
		}else{ //then x == P->r
			if(B->c() == BLACK){ //then  D is RED, symmetric to 1st case
				D->setC(P->c());
				x = leftlongrot(P);
			}else{  //then B is RED, symmetric to 2nd case
				C->setC(P->c());
				B->setC(BLACK);
				x = rightrot(P);
			}
		}
		P->setC(BLACK);
	}else{//then C is RED
		/*     now may be 6 cases:                                   */
		/*       1               2                 3                 */
		/*      [P]             [P]               [P]                */
		/*     /   \           /   \             /   \       AND 3   */
		/*  [[x]]  (C)  OR  [[x]]  (C)   OR   [[x]]  (C)   SYMMETRIC */
		/*         / \             / \               / \     CASES   */
		/*       [B] [D]         [B] [D]           [B] [D]           */
		/*       / \             / \               / \               */
		/*     [s] [t]         (s)  t            [s] (t)             */
		/*                                                           */
		/*   [...] -- BLACK node, (...) -- RED node                  */
		/*   s,t -- any subtrees                                     */
		if(x == P->left()){
			if(B->right()->c()==BLACK){
				if(B->left()->c()==BLACK){  /*      [P]                  [C]     */
					x = leftrot(P);         /*     /   \                /   \    */
					B->setC(RED);           /*  [[x]]  (C)            [P]   [D]  */
					C->setC(BLACK);         /*         / \    --->    / \        */
					/* */                   /*       [B] [D]        [x] (B)      */
					/* */                   /*       / \                / \      */
					/* */                   /*     [s] [t]            [s] [t]    */
				}else{ //B->l is RED, 2nd case
					A = B->left();          /*     [P]                    [C]    */
					C->setC(BLACK);         /*    /   \                  /   \   */
					P->setR(A->left());     /* [[x]]  (C)              (A)    v  */
					B->setL(A->right());    /*        / \             /   \      */
					C->setL(A);             /*      [B]  v   --->   [P]   [B]    */
					A->setR(B);             /*      / \             / \   / \    */
					A->setL(P);             /*    (A)  u          [x]  s t   u   */
					A->setP(C);             /*    / \                            */
					P->right()->setP(P);    /*   s   t                           */
					B->left()->setP(B);     /*                                   */
					P->setP(A); B->setP(A); /*   s,t,u,v -- any subtrees         */
					x = C;
				}
			}else{ //C->r is RED, 3rd case
				B->setC(RED);               /*      [P]                   [C]    */
				C->setC(BLACK);             /*     /   \                 /   \   */
				B->right()->setC(BLACK);    /*  [[x]]  (C)             (B)    u  */
				P->setR(B->left());         /*         / \    --->     / \       */
				B->setL(P);                 /*       [B]  u          [P] [t]     */
				P->setP(B);                 /*       / \             / \         */
				P->right()->setP(P);        /*      s  (t)         [x]  s        */
				x = C;                      /*                                   */
				/* */                       /*   s,t,u -- any subtrees           */
			}
		}else{// then x == P->r
			if(D->left()->c()==BLACK){
				if(D->right()->c()==BLACK){  //symmetric to 1st case
					x = rightrot(P);
					D->setC(RED); C->setC(BLACK);
				}else{               //symmetric to 2nd case
					E = D->right();
					C->setC(BLACK);         /*     [P]               [C]         */
					C->setR(E);             /*    /   \             /   \        */
					P->setL(E->right());    /*  (C)  [[x]]         s    (E)      */
					D->setR(E->left());     /*  / \                    /   \     */
					E->setR(P);             /* s  [D]       --->     [D]   [P]   */
					E->setL(D);             /*    / \                / \   / \   */
					D->right()->setP(D);    /*   t  (E)             t   u v  [x] */
					E->setP(C);             /*      / \                          */
					P->setP(E); D->setP(E); /*     u   v                         */
					P->left()->setP(P);     /*                                   */
					x = C;                  /*   s,t,u,v -- any subtrees         */
				}
			}else{ //C->r is RED,    symmetric to 3rd case
				D->setC(RED);	            /*     [P]               [C]         */
				C->setC(BLACK);             /*    /   \             /   \        */
				D->left()->setC(BLACK);     /*  (C)  [[x]]         s    (D)      */
				P->setL(D->right());        /*  / \         --->       /   \     */
				D->setR(P);                 /* s  [D]                [t]   [P]   */
				/* */                       /*    / \                      / \   */
				P->setP(D);                 /*  (t)  u                    u  [x] */
				P->left()->setP(P);         /*                                   */
				x = C;                      /*   s,t,u -- any subtrees           */
			}
		}
	}
	if(P == pp->left())pp->setL(x);
	else pp->setR(x);
	x->setP(pp);
}
