#pragma once
template<class Node>
Node *nextNode(Node *n){
	if(n->r->isNil()){
		do{
			n = n->p;
		}while(!n->isNil() && n->r->p == n);
		return n;
	}
//	n = n->r;
//	if(!n->isNil() && !n->r->isNil())
		for(n=n->r; !n->l->isNil();)
			n=n->l;
	return n;
}

template<class Node>
Node *nextNodeF(Node *n){
	if (n->right()->isNil()){
		do{
			n = n->up();
		} while (!n->isNil() && n->right()->up() == n);
		return n;
	}
	//	n = n->r;
	//	if(!n->isNil() && !n->r->isNil())
	for (n = n->right(); !n->left()->isNil();)
		n = n->left();
	return n;
}

template<class Node>
Node *prevNode(Node *n){
	while(!n->isNil()){
		if(n->p->r == n){
			n = n->p;
			break;
		}else n = n->p;
	}
	if(!n->isNil() && !n->l->isNil())
		for(n=n->l;!n->r->isNil();)
			n=n->r;
	return n;
}

template<class Node>
Node *prevNodeF(Node *n){
	while (!n->isNil()){
		if (n->up()->right() == n){
			n = n->up();
			break;
		}
		else n = n->up() ;
	}
	if (!n->isNil() && !n->left()->isNil())
	for (n = n->left(); !n->right()->isNil();)
		n = n->right();
	return n;
}

template<class Key, class Node, class Cmp>
const Node *findLeastRight(const Key& key, const Node *nil, const Cmp &cmp){
	const Node *pn = nil, *n = (Node*)pn->l;
	while(n!=nil){
		int c = cmp.compare(n->key, key);
		if(c<0)n = (Node*)n->r;
		else if(c>0)pn = n, n = (Node*)n->l;
		else return n;
	}
	return pn;
	(void)cmp;
}
template<class Key, class Node, class Cmp>
const Node *findLeastRightF(const Key& key, const Node *nil, const Cmp &cmp){
	Node *pn = (Node*)nil, *n = (Node*)pn->left();
	while (n != nil){
		int c = cmp.compare(n->key, key);
		if (c<0)n = (Node*)n->right();
		else if (c>0)pn = n, n = (Node*)n->left();
		else return n;
	}
	return pn;
	(void)cmp;
}
template<class Key, class Node, class Cmp>
const Node *findMostLeft(const Key& key, const Node *nil, const Cmp &cmp){
	const Node *pn = nil, *n = (Node*)pn->l;
	while(n!=nil){
		int c = cmp.compare(n->key, key);
		if(c<0)pn = n, n = (Node*)n->r;
		else if(c>0)n = (Node*)n->l;
		else return n;
	}
	return pn;
	(void)cmp;
}
template<class Key, class Node, class Cmp>
const Node *findMostLeftF(const Key& key, const Node *nil, const Cmp &cmp){
	Node *pn = (Node*)nil, *n = pn->left();
	while (n != nil){
		int c = cmp.compare(n->key, key);
		if (c<0)pn = n, n = (Node*)n->right();
		else if (c>0)n = (Node*)n->left();
		else return n;
	}
	return pn;
	(void)cmp;
}
template<class Key, class Node, class Cmp>
const Node *findIdentical(const Key& key, const Node *nil, const Cmp &cmp){
	const Node *n = (Node*)nil->l;
	while(n!=nil){
		int c = cmp.compare(n->key, key);
		if(c<0)n = (Node*)n->r;
		else if(c>0)n = (Node*)n->l;
		else return n;
	}
	return nil;
	(void)cmp;
}
template<class Key, class Node, class Cmp>
const Node *findIdenticalF(const Key& key, const Node *nil, const Cmp &cmp){
	Node *n = (Node*)((Node*)nil)->left();
	while (n != nil){
		int c = cmp.compare(n->key, key);
		if (c<0)n = (Node*)n->right();
		else if (c>0)n = (Node*)n->left();
		else return n;
	}
	return nil;
	(void)cmp;
}
template<class Node>
struct insertPlaceResult{
	Node * n;
	int c;
	insertPlaceResult(){}
	insertPlaceResult(Node *nn, int cc=0){n=nn; c=cc;}
};
template<class K1, class Node, class Cmp>
insertPlaceResult<Node> findInsertPlace(const K1 &key, Node *nil, const Cmp &cmp){
	Node *n = (Node*)nil->l, *pn = nil;
	int c=0;
	while(n!=nil){
		pn = n;
		c = cmp.compare(n->key, key);
		if(c<0)n = (Node*)n->r;
		else if(c>0)n = (Node*)n->l;
		else return n;
	}
	return insertPlaceResult<Node>(pn,c);
	(void)cmp;
}
template<class K1, class Node, class Cmp>
insertPlaceResult<Node> findInsertPlaceF(const K1 &key, Node *nil, const Cmp &cmp){
	Node *n = (Node*)nil->left(), *pn = nil;
	int c = 0;
	while (n != nil){
		pn = n;
		c = cmp.compare(n->key, key);
		if (c<0)n = (Node*)n->right();
		else if (c>0)n = (Node*)n->left();
		else return n;
	}
	return insertPlaceResult<Node>(pn, c);
	(void)cmp;
}
