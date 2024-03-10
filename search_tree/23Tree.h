#pragma once
inline int Max(int x, int y){return x<y ? y : x;}
inline int Min(int x, int y){return x<y ? y : x;}

// 2-3 tree
template <class K, class Cmp>
class Tree23Base{
	Cmp cmp;
	size_t _node_c = 0, _elem_c = 0;
	struct elem{
		K k1, k2;
		elem * r, * p;
		static elem Nil;// (r == &Nil) <=> (in this node only 1 key)
		elem(){
			p=0;
			r = &Nil;
		}
		explicit elem(const K & k, elem * parent=0){
			k1 = k;
			r=&Nil;
			p=parent;
		}
		elem * init(){p=0; r = &Nil;return this;}
		elem * init(const K & k, elem * parent=0){
			k1 = k;
			r=&Nil;
			p=parent;
			return this;
		}
		bool full() const{return r!=&Nil;}
	};
	struct node : public elem{
		elem * l, * m;
		node():elem(){l=m=0;}
		node(const K & k, elem * parent, elem * left, elem * mid):elem(k,parent){
			if((l=left)!=0)l->p = this;
			if((m=mid)!=0)m->p = this;
		}
		node* init(const K & k, elem * parent, elem * left, elem * mid){
			elem::init(k, parent);
			if((l=left)!=0)l->p = this;
			if((m=mid)!=0)m->p = this;
			return this;
		}
	};
	using _pn = node*;


	//===========================
	static void delete_subtree(elem * root, int d){
		if (d){ //if root not a leaf
			delete_subtree(((node*)root)->l, d - 1);
			delete_subtree(((node*)root)->m, d - 1);
			if (root->r != &elem::Nil)delete_subtree(((node*)root)->r, d - 1);
			delete root;
		}
		else delete root;
	}
	//===========================
	void _insert(elem * e, const K & x, elem * re){
		if(!e){
			if(root){
                root = new node(x, 0, root, re);
                _node_c++;
            } else {
                root = new elem(x);
                _elem_c++;
            }
			_depth++;
			return;
		}
		if(e->r != &elem::Nil)return _split(e, x, re);
		if(cmp.compare(x, e->k1)>0){
			e->k2 = x;
			if((e->r = re)!=0)re->p = e;
			return;
		}
		e->k2 = e->k1;
        e->k1 = x;
		if(re){
			e->r = _pn(e)->m;
			_pn(e)->m = re;
			re->p = e;
		}else e->r = 0;
	}
	//===========================
	void _split(elem * e, const K & x, elem * re);
	//===========================
	void _delete(elem * e, elem * el);
/*	//===========================
	bool check_p(elem * root){
		if(!root||root == &elem::Nil)return true;
		if(root == this->root&&root->p!=0)return false;
		if(root->l && (root->l->p!=root || root->m->p!=root))return false;
		if(root->r&&root->r!=&elem::Nil&&root->r->p!=root)return false;
		return check_p(root->l)&&check_p(root->m)&&check_p(root->r);
	}
/*/	//===========================
	int n_nodes(elem * root, int d){
		if(!root||root==&elem::Nil)return 0;
		if(!d)return 2*sizeof(K)+2*sizeof(void*);
		return 2*sizeof(K)+4*sizeof(void*)+n_nodes(_pn(root)->l,d-1)+n_nodes(_pn(root)->m,d-1)+n_nodes(root->r,d-1);
	}
//*/
	size_t _size = 0;
	unsigned _depth = 0;
	elem * root = nullptr;
	static int _nop;
public:
	static int reset_N(int val=0){
		int res = _nop;
		_nop = val;
		return res;
	}
	static int get_N(){return _nop;}

	int n_nodes(){return n_nodes(root, _depth-1);}
	Tree23Base(){ _node_c = _elem_c = 0; }
    // Destructor
	~Tree23Base(){
		if(root)delete_subtree(root, _depth-1);
		root = nullptr;
        _node_c = _elem_c = 0;
	}
	bool insert(const K& k);
	template<class K1>
	bool erase(const K1& k);
	size_t size()const {return _size;}
	template<class K1>
	K* findT(const K1& e);
	template<class K1>
	const K* findRightT(const K1& e)const;
	template<class K1>
	const K* findLeftT(const K1& e)const;
	unsigned int depth() const {
		return _depth ? _depth - 1 : 0;
	}

	double fill_coef()const{
		return 0.5*double(size) / double(_node_c + _elem_c);
	}
	size_t memory_usage()const{
		return _node_c*sizeof(node)+_elem_c*sizeof(elem);
	}
};
template<class K, class V>
struct KVPair{
    K k;
    V v;
    KVPair() = default;
    explicit KVPair(const K&_k):k(_k){}
    KVPair(const K&_k, const V &_v):k(_k),v(_v){}
};

template<class K, class V, class Cmp>
struct KVCompare : protected Cmp{
	template<class K1>
	int compare(const KVPair<K,V> &p, const K1 &k)const{return Cmp::compare(p.k, k);}
	int compare(const KVPair<K,V> &p1, const KVPair<K,V> &p2)const{return Cmp::compare(p1.k, p2.k);}
};

template<class K, class V, class Cmp>
class Tree23 : protected Tree23Base<KVPair<K,V>, KVCompare<K,V,Cmp> >{
	typedef Tree23Base<KVPair<K,V>, KVCompare<K,V,Cmp> > _base;
public:
    Tree23() = default;
	using _base::size;

	bool insert(const K& k, const V &v){
        return _base::insert(KVPair<K,V>(k,v));
    }
	bool remove(const K& k){
        return this->erase(k);
    }
	V* find(const K& k){
		KVPair<K,V> *p = this->findT(k);
		return p ? &p->v : 0; 
	}
	const V* findRight(const K& k)const{
		const KVPair<K,V> *p = this->findRightT(k);
		return p ? &p->v : 0; 
	}
	const V *end()const{ return 0; }
	size_t memory_usage()const{
		return _base::memory_usage();
	}
};

template<class K,class Cmp>
int Tree23Base<K,Cmp>::_nop=0;

template<class K, class Cmp>
template<class K1>
K* Tree23Base<K, Cmp>::findT(const K1 & e){
	if(!root)return 0;
	elem * curr = root;
	for(size_t i = 1; i<_depth; i++){
		int r = cmp.compare(curr->k1,e);
		if(r>0)curr = _pn(curr)->l;
		else if(r<0){
			if((curr->r == &elem::Nil) || ((r = cmp.compare(curr->k2,e)) > 0))curr = _pn(curr)->m;
			else if(r<0)curr = curr->r;
			//else &curr->k2; //e = k2
		}//else &curr->k1; //e = k1
	}
	int r1;
	if(!(r1=cmp.compare(curr->k1,e)))return &curr->k1;
	if((curr->full()&&r1<0&&!cmp.compare(curr->k2,e)))return &curr->k2;
	return 0;
}

template<class K, class Cmp>
	template<class K1>
const K* Tree23Base<K, Cmp>::findRightT(const K1 & e)const{
	const K *res=0;
	const elem * curr = root;
	for(size_t i = 1; i<_depth; i++, _nop++){
		if(cmp.compare(curr->k1,e)<0){
			if(curr->r == &elem::Nil)curr = _pn(curr)->m;
			else{
				_nop++;
				if(cmp.compare(curr->k2,e) < 0)curr=curr->r;
				else{//e <= curr->k2
					res = &curr->k2;
					curr = _pn(curr)->m;
				}
			}
		}else{//e <= curr->k1){
			res = &curr->k1;
			curr = _pn(curr)->l;
		}
	}
	_nop++;
	if(cmp.compare(curr->k1,e)>0)return &curr->k1;
	if(curr->full()&&(_nop++, (cmp.compare(curr->k2,e)>0)))return &curr->k2;
	return res;
}

template<class K, class Cmp>
template<class K1>
const K* Tree23Base<K, Cmp>::findLeftT(const K1 & e)const{
	const elem * curr = root;
	const K *res=0;
	for(size_t i = 1; i<_depth; i++, _nop++){
		if(cmp.compare(curr->k1,e)>0)curr = _pn(curr)->l;
		else if(curr->r == &elem::Nil || (_nop++, (cmp.compare(curr->k2,e)>0))){
			curr = _pn(curr)->m;
			res = &curr->k1;
		}else{
			res = &curr->k2;
			curr = curr->r;
		}
	}
	if(cmp.compare(curr->k1,e)>0)return res;
	if(!curr->full()||cmp.compare(curr->k2,e)>0)return &curr->k1;
	return &curr->k2;
}


template<class K, class Cmp>
bool Tree23Base<K, Cmp>::insert(const K & k){
	elem * curr = root;
	for(size_t i = 1; i < _depth; i++){
		int r = cmp.compare(curr->k1,k);
		if(r>0)curr = _pn(curr)->l;
		else if(r<0){
			if((curr->r == &elem::Nil) || ((r = cmp.compare(curr->k2,k)) > 0))curr = _pn(curr)->m;
			else if(r < 0)curr = curr->r;
			else return false; //e = k2
		}else return false; //e = k1
	}
	int r1;
	if(curr){
		if(!(r1=cmp.compare(curr->k1,k))){
			curr->k1 = k;
			return false;
		}else if(curr->full()&&r1>0&&!cmp.compare(curr->k2,k)){
			curr->k2 = k;
			return false;
		}
	}
    _insert(curr, k, 0);
	_size++;
	return true;
}

template<class K, class Cmp>
template<class K1>
bool Tree23Base<K, Cmp>::erase(const K1 & k){
	if(!root)return false;
	size_t i;
	elem * curr = root, * p;
	K * del = 0;
	for(i=1; i<_depth; i++){
		int r = cmp.compare(curr->k1, k);
		if(r>0)curr = _pn(curr)->l;
		else if(r<0){
			if((curr->r == &elem::Nil) || ((r = cmp.compare(curr->k2,k)) > 0))curr = _pn(curr)->m;
			else if(r<0)curr = curr->r;
			else {del = &curr->k2; break;} //e = k2
		}else {del = &curr->k1; break;} //e = k1
	}
	if(i>=_depth){
		if(!cmp.compare(curr->k1,k))del = &curr->k1;
		else if(curr->full()&&!cmp.compare(curr->k2,k))del = &curr->k2;
		else return false;
	}else{
		for(p = (del==&curr->k2) ? curr->r : _pn(curr)->m; ++i<_depth;)p = _pn(p)->l;
		curr = p;
		{
			K t = *del; *del = p->k1; p->k1 = *del;
		}
		//Exchange(*del, p->k1);
		del = &p->k1;
	}
	_size--;
	if(curr->full()){
		if(del == &curr->k1)curr->k1 = curr->k2;
		curr->r = &elem::Nil;
		return true;
	}
    _delete(curr, 0);
	return true;
}

template<class K, class Cmp>
void Tree23Base<K, Cmp>::_split(typename Tree23Base<K, Cmp>::elem * e,
                                const K & x, elem * re){
	K t;
	elem * new_el;
	if(cmp.compare(x,e->k1)<0){
		if(re){
			new_el = new node(e->k2, 0, _pn(e)->m, e->r);
			_pn(e)->m = re;
			re->p = e;
		}else {
            new_el = new elem(e->k2);
        }
        _elem_c++;
		t = e->k1;
		e->k1 = x;
	} else if(cmp.compare(x,e->k2)>0) {
		t = e->k2;
		if(re) {
            new_el = new node(x, 0, e->r, re);
            _node_c++;
        } else {
            new_el = new elem(x);
            _elem_c++;
        }
	} else {
		t = x;
		if(re) {
            new_el = new node(e->k2, 0, re, e->r);
            _node_c++;
        } else {
            new_el = new elem(e->k2);
            _elem_c++;
        }
	}
	e->r = &elem::Nil;
    _insert(e->p, t, new_el);
}

template<class K, class Cmp>
void Tree23Base<K, Cmp>::_delete(typename Tree23Base<K, Cmp>::elem * e,
                                 typename Tree23Base<K, Cmp>::elem * el){
	if(!e->p){
		if(--_depth){
			(root = el)->p = 0;
			delete e;
            _node_c--;
		}
		else{
			root = 0;
			delete e;
            _elem_c--;
		}
		return;
	}
	node * p = _pn(e->p);
	elem * m, * l, * r;
	if(p->l == e){
		m = p->m;
		if(m->full()){
			e->k1 = p->k1;
			p->k1 = m->k1;
			if(el){
				(_pn(e)->m = _pn(m)->l)->p = e;
				_pn(m)->l = _pn(m)->m;	    _pn(m)->m = m->r;
			}
			m->k1=m->k2;
			m->r = &elem::Nil;
			return;
		}
		e->k1 = p->k1;
		e->k2 = m->k1;
		if(el){
			(_pn(e)->m = _pn(m)->l)->p = e;
			(e->r = _pn(m)->m)->p = e;
			delete m;
            _node_c--;
		}else{
			e->r = 0;
			delete m;
            _elem_c--;
		}
		if(p->full()){
			p->k1 = p->k2;
			p->m = p->r;
			p->r = &elem::Nil;
			return;
		}
        _delete(p, e);
	}else if(p->m == e){
		l = p->l;
		if(l->full()){
			if(el){
				_pn(e)->m = el;
				(_pn(e)->l = _pn(l)->r)->p = e;
			}
			e->k1 = p->k1;
			p->k1 = l->k2;
			l->r = &elem::Nil;
			return;
		}
		if((r=p->r) == &elem::Nil){
			l->k2 = p->k1;
			if((l->r = el)!=0){
				el->p = l;
				delete e; _node_c--;
			}
			else { delete e; _elem_c--; }
			return _delete(p, l);
		}
		if(r->full()){
			e->k1 = p->k2;
			p->k2 = r->k1;
			r->k1=r->k2;
			if(el){
				(_pn(e)->m = _pn(r)->l)->p = e;
				_pn(r)->l = _pn(r)->m;	    _pn(r)->m = r->r;
			}
			r->r = &elem::Nil;
			return;
		}
		e->k1 = p->k2;
		e->k2 = r->k1;
		if(el){
			(_pn(e)->m = _pn(r)->l)->p = e;
			(e->r = _pn(r)->m)->p = e;
			delete r; _node_c--;
		}else{
			e->r = 0;
			delete r; _elem_c--;
		}
		p->r = &elem::Nil;
	} else {
		m = p->m;
		if(m->full()){
			e->k1 = p->k2;
			p->k2 = m->k2;
			if(el){
				_pn(e)->m = el;
				(_pn(e)->l = m->r)->p = e;
			}
			m->r = &elem::Nil;
			return;
		}
		m->k2 = p->k2;
		if((m->r = el)!=0){
			el->p = m;
			delete e; _node_c--;
		}
		else {
            delete e;
            _elem_c--;
        }
		p->r = &elem::Nil;
	}
}

template<class K, class Cmp>
typename Tree23Base<K, Cmp>::elem Tree23Base<K, Cmp>::elem::Nil;

