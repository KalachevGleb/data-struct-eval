#include "avltree.h"
#include "commontree.h"
//#include "input1d.h"
//#include "input2d.h"
//#include <stdio.h>
inline int Max(int i, int j){return i>j ? i : j;}
inline int Abs(int i){return i>=0 ? i : -i;}
AVLTreeBase::AVLTreeBase(){
	nil.setL(&nil); nil.setR(&nil); nil.setP(&nil);
	nil.setH(0);
	sz = 0;
	//printf("avl node size = %d\n", (int)sizeof(AVLNodeBase));
}

void AVLTreeBase::_insert(AVLTreeBase::AVLNodeBase *p, AVLTreeBase::AVLNodeBase *n, bool l){
	if(l)p->setL(n);
	else p->setR(n);
	n->setP(p);
	n->setL(&nil); n->setR(&nil);
	n->setH(1);
	this->sz++;
/*	AVLNodeBase *bp = before->prev;
	if(before->l->h){
		n->p = bp;
		bp->r = n;
	}else{
		n->p = before;
		before->l = n;
	}
	n->l = n->r = &nil;
	n->next = before; n->prev = bp;
	before->prev = n; bp->next = n;
	n->h = 1;*/
	_balance(n->up());
}

void AVLTreeBase::_remove(AVLTreeBase::AVLNodeBase *n){
	this->sz--;
	if(n->right()->height()){
		AVLNodeBase *d = n->next();//n->next;
		//(d->prev = n->prev)->next = d;
		if(n->right()->left()->height()){
			AVLNodeBase *p = d->up();
			p->setL(d->right()); p->left()->setP(p);       /*     n___              d___      */
			d->setP(n->up()); d->setH(n->height());        /*    /    \            /    \     */
			d->setL(n->left())->setP(d);                   /*   a     /\          a     /\    */
			d->setR(n->right())->setP(d);                  /*        p...   -->        p...   */
			if (n->up()->left() == n)n->up()->setL(d);     /*       / \..\            / \..\  */
			else n->up()->setR(d);                         /*      d   ...           b   ...  */
											               /*       \                         */
			_balance(p);                                   /*        b                        */
		} else {
			d->setH(n->height());                          /*     n                     */
			d->setP(n->up()); d->setL(n->left())->setP(d); /*    / \              d     */
			if(n->up()->left()==n)n->up()->setL(d);        /*   a   d     -->    / \    */
			else n->up()->setR(d);                         /*        \          a   c   */
			_balance(d);                                   /*         c                 */
		}
	}else{
		AVLNodeBase *p = n->up();
	//	n->next->prev = n->prev;
	//	n->prev->next = n->next;
		if (p->left() == n) p->setL(n->left());
		else p->setR(n->left());
		n->left()->setP(p);
		_balance(p);
	}
}
//#include <stdio.h>
//AVL balancing
void AVLTreeBase::_balance(AVLTreeBase::AVLNodeBase *x){
//	printf("0");
	for(;x->height(); x = x->up()){
		//printf("0");
		int ph = x->height();
		if(x->left()->height() == x->right()->height()+2){ //
		//	printf("0\n");
			AVLNodeBase *d = x->left(), *c = d->right(), *e = d->left();// *a = x->r;
			if(c->height() <= e->height()){
				if(x->up()->left() == x)x->up()->setL(d);  /*     x             d       */
				else x->up()->setR(d);                     /*    / \           / \      */
				d->setR(x); d->setP(x->up()); x->setP(d);  /*   d   a   -->   e   x     */
				x->setL(c);   c->setP(x);                  /*  / \               / \    */
				x->setH(c->height() + 1);                  /* e   c             c   a   */
				d->setH(x->height() + 1);
				x = d;
			} else {//c->h > e->h, x->h = d->h+1 = c->h+2 = e->h+3;
				AVLNodeBase *cl = c->left(),
					*cr = c->right();                      /*      x               c        */
				if(x->up()->left() == x)x->up()->setL(c);  /*     / \            /   \      */
				else x->up()->setR(c);                     /*    d   a  -->     d     x     */
				c->setR(x); c->setP(x->up()); x->setP(c);  /*   / \            / \   / \    */
				c->setL(d); d->setP(c);                    /*  e   c          e  cl cr  a   */
				cr->setP(x); x->setL(cr);                  /*     / \                       */
				cl->setP(d); d->setR(cl);                  /*    cl cr                      */

				x->setH(c->height()); d->setH(c->height()); c->height()++;
				x = c;
			}
		}else if(x->left()->height()+2 == x->right()->height()){//
		//	printf("1\n");
			AVLNodeBase *d = x->right(), *c = d->left(), *e = d->right();//*a = x->l;
			if(c->height() <= e->height()){
				if(x->up()->left() == x)x->up()->setL(d);  /*     x               d    */
				else x->up()->setR(d);                     /*    / \             / \   */
				d->setL(x); d->setP(x->up()); x->setP(d);  /*   a   d    -->    x   e  */
				x->setR(c); c->setP(x);                    /*      / \         / \     */
				x->setH(c->height() + 1);                  /*     c   e       a   c    */
				d->setH(x->height() + 1);
				x = d;
			} else {//c->h > e->h, x->h = d->h+1 = c->h+2 = e->h+3;
				AVLNodeBase *cl = c->left(),
					*cr = c->right();                      /*    x                 c        */
				if (x->up()->left() == x)x->up()->setL(c); /*   / \              /   \      */
				else x->up()->setR(c);                     /*  a   d    -->     x     d     */
				c->setL(x); c->setP(x->up()); x->setP(c);  /*     / \          / \   / \    */
				c->setR(d); d->setP(c);                    /*    c   e        a  cl cr  e   */
				cl->setP(x); x->setR(cl);                  /*   / \                         */
				cr->setP(d); d->setL(cr);                  /*  cl cr                        */

				x->setH(d->height() = c->height()++);
				x = c;
			}
		}else x->setH(Max(x->left()->height(), x->right()->height())+1);
		if(ph == x->height())break;
	}
}
