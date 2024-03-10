#include "Btree.h"

BTreeBase::BTreeBase(){
	h = 0;
	root = 0;
	B = _2B = 0;
	key_size = 0;
	_sz = 0;
	check_l = check_r = true;
}

BTreeBase::~BTreeBase(){
//	delete[] mbuf;
}

void BTreeBase::_setKVSize(size_t ksz, size_t allsz){
#ifdef _DEBUG
	if (_sz)throw "BTree::_setKVSize : attempt to change element sizes in non-empty B-tree";
#endif
	key_size = (int)ksz;
	f_leaf_size = (int)allsz;
	f_size = f_leaf_size + sizeof(void*);
//	delete[] mbuf;
//	mbuf = new char[allsz];
}


void BTreeBase::setB(int b){
#ifdef _DEBUG
	if (_sz)throw "BTree::setB : attempt to change B in non-empty B-tree";
#endif
	if (B == b)return;
	B = b; _2B = 2 * b;
	_nodes.setObjSize(node_size());
	_leaves.setObjSize(leaf_size());
}

BTreeBase::_findres BTreeBase::_insert_leaf(BTreeBase::BNode *into, void *k, unsigned int pos){
//	memcpy(mbuf, k, key_size);
	++_sz;
	if (into){
		if (into->knum == _2B){
			BNode *l, *r, *p = into->p;
			unsigned int pp = into->pnum;
			if (p){
				if (check_l && pp && (l = down(p, pp - 1))->knum < _2B){
					memcpy(end_leaf_data(l), key_node_ptr(p, pp - 1), key_size);
					l->knum++;
					if (!pos){
						memcpy(key_node_ptr(p, pp - 1), k, key_size);
						return _findres(p, pp - 1, true);
					}
					else {
						memcpy(key_node_ptr(p, pp - 1), key_leaf_ptr(into, 0), key_size);
						move_leafdata(into, 1, pos, -1);
						memcpy(key_leaf_ptr(into, pos - 1), k, key_size);
						return _findres(into, pos - 1, true);
					}
				}
				else if (check_r && pp < p->knum && (r = down(p, pp + 1))->knum < _2B){
					move_leafdata(r, 0, r->knum, 1);
					r->knum++;
					memcpy(key_leaf_ptr(r, 0), key_node_ptr(p, pp), key_size);
					if (pos == _2B){
						memcpy(key_node_ptr(p, pp), k, key_size);
						return _findres(p, pp, true);
					}
					else{
						memcpy(key_node_ptr(p, pp), key_leaf_ptr(into, _2B - 1), key_size);
						move_leafdata(into, pos, _2B - 1, 1);
						memcpy(key_leaf_ptr(into, pos), k, key_size);
						return _findres(into, pos, true);
					}
				}
			}
			BNode *n = new_leaf();
			n->knum = into->knum = B;
			n->p = p;
			_findres res(into, pos, true);
			if (pos <= B){
				memcpy(n->data, into->data + B*key_size, B*key_size);
				move_leafdata(into, pos, B, 1);
				memcpy(key_leaf_ptr(into, pos), k, key_size);
			}
			else{ // pos >= B+1
				int p1 = pos - B - 1;
				memcpy(n->data, into->data + (B + 1)*key_size, p1*key_size);
				memcpy(key_leaf_ptr(n, p1), k, key_size);
				memcpy(key_leaf_ptr(n, p1 + 1), key_leaf_ptr(into, pos), (_2B - pos)*key_size);
				res.n = n;
				res.pos = p1;
			}
			_insert(p, (char*)key_leaf_ptr(into, B), pp, n);
			if (pos == B){
				for (res.n = into->p, res.pos = into->pnum; res.n->knum <= res.pos;){
					res.pos = res.n->pnum; res.n = res.n->p;
				}
			}
			return res;
		}
		else { //into->knum < 2*B;
			move_leafdata(into, pos, into->knum, 1);
			memcpy(key_leaf_ptr(into, pos), k, key_size);
			into->knum++;
			return _findres(into, pos, true);
		}
	}
	else{ // into = 0 => root = 0
		root = new_leaf();
		root->knum = 1;
		root->setp(0, 0);
		memcpy(root->data, k, key_size);
		h = 1;
		return _findres(root, 0, true);
	}
}

void BTreeBase::_insert(BTreeBase::BNode *into, char *buf, unsigned int pos, BTreeBase::BNode *y){
	while(into && into->knum == _2B){
		BNode *l, *r, *p = into->p;
		unsigned int pp = into->pnum;
		if (p){
			if (check_l && pp && (l = down(p, pp - 1))->knum < _2B){
				memcpy(end_node_data(l), key_node_ptr(p, pp - 1), key_size);
				l->knum++;
				set_down(l, l->knum, down(into, 0));
				if (!pos){
					memcpy(key_node_ptr(p, pp - 1), buf, key_size);
				}
				else {
					memcpy(key_node_ptr(p, pp - 1), key_node_ptr(into, 0), key_size);
					set_down(into, 0, down(into, 1));
					move_rdata(into, 1, pos, -1);
					memcpy(key_node_ptr(into, pos - 1), buf, key_size);
				}
				set_down(into, pos, y);
				return;
			}
			else if (check_r && pp < p->knum && (r = down(p, pp + 1))->knum < _2B){
				move_rdata(r, 0, r->knum, 1);
				set_down(r, 1, down(r, 0));
				r->knum++;
				memcpy(key_node_ptr(r, 0), key_node_ptr(p, pp), key_size);
				if (pos == _2B){
					memcpy(key_node_ptr(p, pp), buf, key_size);
					set_down(r, 0, y);
				}
				else{
					memcpy(key_node_ptr(p, pp), key_node_ptr(into, _2B - 1), key_size);
					set_down(r, 0, down(into, _2B));
					move_rdata(into, pos, _2B - 1, 1);
					memcpy(key_node_ptr(into, pos), buf, key_size);
					set_down(into, pos + 1, y);
				}
				return;
			}
		}
		BNode *n = new_node();
		n->knum = into->knum = B;
		n->p = p;
		if (pos < B){
			memcpy(n->data, into->data + B*f_size, B*f_size + sizeof(void*));
			move_rdata(into, pos, B, 1);
			memcpy(key_node_ptr(into, pos), buf, key_size);
			set_down(into, pos + 1, y);
		}
		else if (pos == B){
			memcpy(key_node_ptr(n,0), key_node_ptr(into, B), B*f_size);
			memcpy(key_node_ptr(into, pos), buf, key_size);
			set_down(n, 0, y);
		}
		else{ // pos >= B+1
			int p1 = pos - B - 1;
			memcpy(n->data, into->data + (B + 1)*f_size, p1*f_size + sizeof(void*));
			memcpy(key_node_ptr(n, p1), buf, key_size);
			down(n, p1+1) = y;
			memcpy(key_node_ptr(n, p1 + 1), key_node_ptr(into, pos), (_2B - pos)*f_size);
		}
		set_ptrs(n);
		buf = (char*)key_node_ptr(into, B);
		into = p; pos = pp; y = n;
	}
	if (!into){ //root was splitted
		into = new_node();
		into->knum = 1;
		into->setp(0, 0);
		set_down(into, 0, root);
		set_down(into, 1, y);
		memcpy(key_node_ptr(into, 0), buf, key_size);
		root = into;
		h++;
	}
	else { //normal case: into->knum < 2*B;
		move_rdata(into, pos, into->knum, 1);
		memcpy(key_node_ptr(into, pos), buf, key_size);
		set_down(into, pos + 1, y);
		into->knum++;
	}
}

BTreeBase::_findres BTreeBase::_remove_leaf(BTreeBase::BNode *x, unsigned int pos){
	--_sz;
	if (x == root && x->knum == 1){
		free_leaf(x); root = 0;
		return _findres();
	}
	else if(x!=root && x->knum == B){
		BNode *l=0, *r=0, *p = x->p; //x != root => x->p != 0
		unsigned int pp = x->pnum;
		if (pp && (l = down(p, pp - 1))->knum == B){
			memcpy(key_leaf_ptr(l, l->knum), key_node_ptr(p, pp - 1), key_size);
			memcpy(key_leaf_ptr(l, l->knum + 1), x->data, pos*key_size);
			int p1 = l->knum + 1 + pos;
			memcpy(key_leaf_ptr(l, p1), key_leaf_ptr(x, pos + 1), (B - pos - 1)*key_size);
			l->knum += B;
			free_leaf(x);
			_remove(p, pp - 1);
			return _findres(l, p1, true);
		}
		else if (pp<p->knum && (r = down(p, pp + 1))->knum == B){
			move_leafdata(x, pos + 1, B, -1);
			memcpy(key_leaf_ptr(x, B - 1), key_node_ptr(p, pp), key_size);
			memcpy(key_leaf_ptr(x, B), r->data, r->knum*key_size);
			x->knum += r->knum;
			free_leaf(r);
			_remove(p, pp);
			return _findres(x, pos, true);
		}
		else if (pp){// && (l = down(p, pp - 1))->knum > B && (pp==p->knum || down(p,pp+1)->knum > B)){
			move_leafdata(x, 0, pos, 1);
			l->knum--;
			memcpy(key_leaf_ptr(x, 0), key_node_ptr(p, pp - 1), key_size);
			memcpy(key_node_ptr(p, pp - 1), end_leaf_data(l), key_size);
			return _findres(x, pos+1, true);
		}
		else {//if (pp < p->knum && (r = down(p, pp + 1))->knum > B){
			move_leafdata(x, pos + 1, B, -1);
			memcpy(key_leaf_ptr(x, B - 1), key_node_ptr(p, pp), key_size);
			memcpy(key_node_ptr(p, pp), key_leaf_ptr(r, 0), key_size);
			move_leafdata(r, 1, r->knum, -1);
			r->knum--;
			return _findres(x, pos, true);
		}
/*		else if (pp){
			memcpy(key_leaf_ptr(l, l->knum), key_node_ptr(p, pp - 1), key_size);
			memcpy(key_leaf_ptr(l, l->knum + 1), x->data, pos*key_size);
			int p1 = l->knum + 1 + pos;
			memcpy(key_leaf_ptr(l, p1), key_leaf_ptr(x, pos+1), (B-pos-1)*key_size);
			l->knum += B;
			free_leaf(x);
			_remove(p, pp - 1);
			return _findres(l, p1, true);
		}
		else{
			move_leafdata(x, pos + 1, B, -1);
			memcpy(key_leaf_ptr(x, B-1), key_node_ptr(p, pp), key_size);
			memcpy(key_leaf_ptr(x, B), r->data, r->knum*key_size);
			x->knum += r->knum;
			free_leaf(r);
			_remove(p, pp);
			return _findres(x, pos, true);
		}*/
	}
	else{	//normal case: into->knum < 2*B;
		move_leafdata(x, pos+1, x->knum, -1);
		x->knum--;
		return _findres(x, pos, true);
	}
}
void BTreeBase::_remove(BTreeBase::BNode *x, unsigned int pos){
	while (x != root && x->knum == B){
		BNode *l = 0, *r = 0, *p = x->p; //x != root => x->p != 0
		unsigned int pp = x->pnum;
		if (pp && (l = down(p, pp - 1))->knum == B){
			memcpy(key_node_ptr(l, l->knum), key_node_ptr(p, pp - 1), f_leaf_size);
			memcpy(&down(l, l->knum + 1), x->data, pos*f_size + sizeof(void*));
			memcpy(key_node_ptr(l, l->knum + 1 + pos), key_node_ptr(x, pos + 1), (B - pos - 1)*f_size);
			l->knum += B;
			set_ptrs(l, l->knum - B);
			free_node(x);
			x = p; pos = pp - 1;
		}
		else if (pp < p->knum && (r = down(p, pp + 1))->knum == B){
			move_rdata(x, pos + 1, B, -1);
			memcpy(key_node_ptr(x, B - 1), key_node_ptr(p, pp), f_leaf_size);
			memcpy(&down(x, B), r->data, r->knum*f_size + sizeof(void*));
			x->knum += r->knum;
			set_ptrs(x, B);
			free_node(r);
			x = p; pos = pp;
		}
		else if (pp){// && (l = down(p, pp - 1))->knum > B){
			move_rdata(x, 0, pos, 1);
			set_down(x, 1, down(x, 0));
			memcpy(key_node_ptr(x, 0), key_node_ptr(p, pp - 1), f_leaf_size);
			set_down(x, 0, down(l, l->knum));
			l->knum--;
			memcpy(key_node_ptr(p, pp - 1), end_node_data(l), f_leaf_size);
			return;
		}
		else {//if (pp < p->knum && (r = down(p, pp + 1))->knum > B){
			move_rdata(x, pos + 1, B, -1);
			memcpy(key_node_ptr(x, B - 1), key_node_ptr(p, pp), f_leaf_size);
			memcpy(key_node_ptr(p, pp), key_node_ptr(r, 0), f_leaf_size);
			set_down(x, B, down(r, 0));
			set_down(r, 0, down(r, 1));
			move_rdata(r, 1, r->knum, -1);
			r->knum--;
			return;
		}
	}
	if (x == root && x->knum == 1){
		root = down(x, 0);
		root->setp(0, 0);
		free_node(x);
	}
	else { //normal case: into->knum < 2*B;
		move_rdata(x, pos+1, x->knum, -1);
		x->knum--;
	}
}

/// ======================= BTreeVBase ==============================///
BTreeVBase::BTreeVBase(){
	h = 0;
	root = 0;
	B = _2B = 0;
	key_size = val_size = 0;
	val_offset = ptr_offset = 0;
	_sz = 0;
	check_l = check_r = true;
	key_offset = int(((BNode*)0)->data - (char*)0);
}

BTreeVBase::~BTreeVBase(){
	//	delete[] mbuf;
}

void BTreeVBase::_setKVSize(size_t ksz, size_t valsz, size_t v_off){
#ifdef _DEBUG
	if (_sz)throw "BTree::_setKVSize : attempt to change element sizes in non-empty B-tree";
	if (!ksz)throw "BTree::_setKVSize : zero key size";
#endif
	key_size = (int)ksz;
	val_size = (int)valsz;
	val_off0 = (int)v_off;
	val_offset = int(_2B*ksz);
	unsigned int t;
	if (valsz){
		t = (key_offset + val_offset) % val_off0;
		if (t)val_offset += val_off0 - t;
	}
	ptr_offset = int(val_offset+_2B*valsz);
	t = int((key_offset + ptr_offset) % sizeof(void*));
	if (t)ptr_offset += int(sizeof(void*)-t);
	//	delete[] mbuf;
	//	mbuf = new char[allsz];
}


void BTreeVBase::setB(int b){
#ifdef _DEBUG
	if (_sz)throw "BTree::setB : attempt to change B in non-empty B-tree";
#endif
	if (B == b)return;
	B = b; _2B = 2 * b;
	val_offset = _2B*key_size;
	unsigned int t = (key_offset + val_offset) % val_off0;
	if (t)val_offset += val_off0 - t;
	ptr_offset = val_offset + _2B*(val_size);
	t = int((key_offset + ptr_offset) % sizeof(void*));
	if (t)ptr_offset += int(sizeof(void*)-t);
	_nodes.setObjSize(node_size());
	_leaves.setObjSize(leaf_size());
}

BTreeVBase::_findres BTreeVBase::_insert_leaf(BTreeVBase::BNode *into, void *k, void *v, unsigned int pos){
	//	memcpy(mbuf, k, key_size);
	++_sz;
	if (into){
		if (into->knum == _2B){
			BNode *l, *r, *p = into->p;
			unsigned int pp = into->pnum;
			if (p){
				if (check_l && pp && (l = down(p)[pp - 1])->knum < _2B){
					move_kv(l, l->knum, p, pp - 1);
					l->knum++;
					if (!pos){
						set_kv(p, pp - 1, k, v);
						return _findres(p, pp - 1, true);
					}
					else {
						move_kv(p, pp - 1, into, 0);
						move_leafdata(into, 1, pos, -1);
						set_kv(into, pos - 1, k, v);
						return _findres(into, pos - 1, true);
					}
				}
				else if (check_r && pp < p->knum && (r = down(p)[pp + 1])->knum < _2B){
					move_leafdata(r, 0, r->knum, 1);
					r->knum++;
					move_kv(r,0,p,pp);
					if (pos == _2B){
						set_kv(p, pp, k, v);
						return _findres(p, pp, true);
					}
					else{
						move_kv(p, pp, into, _2B - 1);
						move_leafdata(into, pos, _2B - 1, 1);
						set_kv(into, pos, k, v);
						return _findres(into, pos, true);
					}
				}
			}
			BNode *n = new_leaf();
			n->knum = into->knum = B;
			n->p = p;
			_findres res(into, pos, true);
			if (pos <= B){
				memcpy(key_ptr(n, 0), key_ptr(into, B), B*key_size);
				memcpy(val_ptr(n, 0), val_ptr(into, B), B*val_size);
				move_leafdata(into, pos, B, 1);
				set_kv(into, pos, k, v);
			}
			else{ // pos >= B+1
				int p1 = pos - B - 1;
				memcpy(n->data, into->data + (B + 1)*key_size, p1*key_size);
				memcpy(val_ptr(n,0), val_ptr(into,B + 1), p1*val_size);
				set_kv(n, p1, k, v);
				memcpy(key_ptr(n, p1 + 1), key_ptr(into, pos), (_2B - pos)*key_size);
				memcpy(val_ptr(n, p1 + 1), val_ptr(into, pos), (_2B - pos)*val_size);
				res.n = n;
				res.pos = p1;
			}
			_insert(p, key_ptr(into, B), val_ptr(into, B), pp, n);
			if (pos == B){
				for (res.n = into->p, res.pos = into->pnum; res.n->knum <= res.pos;){
					res.pos = res.n->pnum; res.n = res.n->p;
				}
			}
			return res;
		}
		else { //into->knum < 2*B;
			move_leafdata(into, pos, into->knum, 1);
			set_kv(into, pos, k, v);
			into->knum++;
			return _findres(into, pos, true);
		}
	}
	else{ // into = 0 => root = 0
		root = new_leaf();
		root->knum = 1;
		root->setp(0, 0);
		set_kv(root, 0, k, v);
		h = 1;
		return _findres(root, 0, true);
	}
}

void BTreeVBase::_insert(BTreeVBase::BNode *into, void *k, void *v, unsigned int pos, BTreeVBase::BNode *y){
	while (into && into->knum == _2B){
		BNode *l, *r, *p = into->p;
		unsigned int pp = into->pnum;
		if (p){
			if (check_l && pp && (l = down(p)[pp - 1])->knum < _2B){
				move_kv(l, l->knum, p, pp - 1);
				l->knum++;
				set_down(l, l->knum, down(into)[0]);
				if (!pos){
					set_kv(p, pp - 1, k, v);
				}
				else {
					move_kv(p, pp - 1, into, 0);
					move_lrdata(into, 1, pos, -1);
					set_kv(into, pos - 1, k, v);
				}
				set_down(into, pos, y);
				return;
			}
			else if (check_r && pp < p->knum && (r = down(p)[pp + 1])->knum < _2B){
				move_lrdata(r, 0, r->knum, 1);
				r->knum++;
				move_kv(r, 0, p, pp);
				if (pos == _2B){
					set_kv(p, pp, k, v);
					set_down(r, 0, y);
				}
				else{
					move_kv(p, pp, into, _2B - 1);
					set_down(r, 0, down(into)[_2B]);
					move_rdata(into, pos, _2B - 1, 1);
					set_kv(into, pos, k, v);
					set_down(into, pos + 1, y);
				}
				return;
			}
		}
		BNode *n = new_node();
		n->knum = into->knum = B;
		n->p = p;
		if (pos < B){
			memcpy(key_ptr(n,0), key_ptr(into,B), B*key_size);
			memcpy(val_ptr(n, 0), val_ptr(into, B), B*val_size);
			memcpy(down(n), down(into)+ B, (B+1)*sizeof(BNode*));
			move_rdata(into, pos, B, 1);
			set_kv(into, pos, k, v);
			set_down(into, pos + 1, y);
		}
		else if (pos == B){
			memcpy(key_ptr(n, 0), key_ptr(into, B), B*key_size);
			memcpy(val_ptr(n, 0), val_ptr(into, B), B*val_size);
			memcpy(down(n)+1, down(into)+ B+1, B*sizeof(BNode*));
			set_kv(into, pos, k, v);
			set_down(n, 0, y);
		}
		else{ // pos >= B+1
			int p1 = pos - B - 1;
			memcpy(key_ptr(n,0), key_ptr(into,B + 1), p1*key_size);
			memcpy(val_ptr(n, 0), val_ptr(into, B + 1), p1*val_size);
			memcpy(down(n), down(into)+ B + 1, (p1+1)*sizeof(void*));
			set_kv(n, p1, k, v);
			down(n)[p1 + 1] = y;
			memcpy(key_ptr(n, p1 + 1), key_ptr(into, pos), (_2B - pos)*key_size);
			memcpy(val_ptr(n, p1 + 1), val_ptr(into, pos), (_2B - pos)*val_size);
			memcpy(down(n)+ p1 + 2, down(into)+pos+1, (_2B - pos)*sizeof(void*));
		}
		set_ptrs(n);
		k = key_ptr(into, B);
		v = val_ptr(into, B);
		into = p; pos = pp; y = n;
	}
	if (!into){ //root was splitted
		into = new_node();
		into->knum = 1;
		into->setp(0, 0);
		set_down(into, 0, root);
		set_down(into, 1, y);
		set_kv(into, 0, k, v);
		root = into;
		h++;
	}
	else { //normal case: into->knum < 2*B;
		move_rdata(into, pos, into->knum, 1);
		set_kv(into, pos, k, v);
		set_down(into, pos + 1, y);
		into->knum++;
	}
}

BTreeVBase::_findres BTreeVBase::_remove_leaf(BTreeVBase::BNode *x, unsigned int pos){
	--_sz;
	if (x == root && x->knum == 1){
		free_leaf(x); root = 0;
		return _findres();
	}
	else if (x != root && x->knum == B){
		BNode *l = 0, *r = 0, *p = x->p; //x != root => x->p != 0
		unsigned int pp = x->pnum;
		if (pp && (l = down(p)[pp - 1])->knum == B){
			move_kv(l, l->knum, p, pp - 1);
			memcpy(key_ptr(l, l->knum + 1), key_ptr(x, 0), pos*key_size);
			memcpy(val_ptr(l, l->knum + 1), val_ptr(x, 0), pos*val_size);
			int p1 = l->knum + 1 + pos;
			memcpy(key_ptr(l, p1), key_ptr(x, pos + 1), (B - pos - 1)*key_size);
			memcpy(val_ptr(l, p1), val_ptr(x, pos + 1), (B - pos - 1)*val_size);
			l->knum += B;
			free_leaf(x);
			_remove(p, pp - 1);
			return _findres(l, p1, true);
		}
		else if (pp < p->knum && (r = down(p)[pp + 1])->knum == B){
			move_leafdata(x, pos + 1, B, -1);
			move_kv(x, B - 1, p, pp);
			memcpy(key_ptr(x, B), key_ptr(r, 0), r->knum*key_size);
			memcpy(val_ptr(x, B), val_ptr(r, 0), r->knum*val_size);
			x->knum += r->knum;
			free_leaf(r);
			_remove(p, pp);
			return _findres(x, pos, true);
		}else if (pp){// && (l = down(p)[pp - 1])->knum > B){
			move_leafdata(x, 0, pos, 1);
			l->knum--;
			move_kv(x, 0, p, pp - 1);
			move_kv(p, pp - 1, l, l->knum);
			return _findres(x, pos + 1, true);
		}
		else {//if (pp < p->knum && (r = down(p)[pp + 1])->knum > B){
			move_leafdata(x, pos + 1, B, -1);
			move_kv(x, B - 1, p, pp);
			move_kv(p, pp, r, 0);
			move_leafdata(r, 1, r->knum, -1);
			r->knum--;
			return _findres(x, pos, true);
		}
		
	}
	else{	//normal case: into->knum < 2*B;
		move_leafdata(x, pos + 1, x->knum, -1);
		x->knum--;
		return _findres(x, pos, true);
	}
}
void BTreeVBase::_remove(BTreeVBase::BNode *x, unsigned int pos){
	while (x != root && x->knum == B){
		BNode *l = 0, *r = 0, *p = x->p; //x != root => x->p != 0
		unsigned int pp = x->pnum;
		if (pp && (l = down(p)[pp - 1])->knum == B){
			move_kv(l, l->knum, p, pp - 1);
			//			memcpy(&down(l, l->knum + 1), x->data, pos*f_size + sizeof(void*));
			memcpy(key_ptr(l, l->knum + 1), key_ptr(x, 0), pos*key_size);
			memcpy(val_ptr(l, l->knum + 1), val_ptr(x, 0), pos*val_size);
			memcpy(down(l) + l->knum + 1, down(x), (pos + 1)*sizeof(BNode*));

			//			memcpy(key_node_ptr(l, l->knum + 1 + pos), key_node_ptr(x, pos + 1), (B - pos - 1)*f_size);
			int t = l->knum + 1 + pos, s = B - pos - 1;
			memcpy(key_ptr(l, t), key_ptr(x, pos + 1), s*key_size);
			memcpy(val_ptr(l, t), val_ptr(x, pos + 1), s*val_size);
			memcpy(down(l) + t + 1, down(x) + pos + 2, s*sizeof(BNode*));
			l->knum += B;
			set_ptrs(l, l->knum - B);
			free_node(x);
			x = p; pos = pp - 1;
		}
		else if (pp < p->knum && (r = down(p)[pp + 1])->knum == B){
			move_rdata(x, pos + 1, B, -1);
			move_kv(x, B - 1, p, pp);
			//			memcpy(&down(x, B), r->data, r->knum*f_size + sizeof(void*));
			memcpy(key_ptr(x, B), key_ptr(r, 0), r->knum*key_size);
			memcpy(val_ptr(x, B), val_ptr(r, 0), r->knum*val_size);
			memcpy(down(x) + B, down(r), (r->knum + 1)*sizeof(BNode*));
			x->knum += r->knum;
			set_ptrs(x, B);
			free_node(r);
			x = p; pos = pp;
		}
		else if (pp){// && (l = down(p)[pp - 1])->knum > B){
			move_rdata(x, 0, pos, 1);
			set_down(x, 1, down(x)[0]);
			move_kv(x, 0, p, pp - 1);
			set_down(x, 0, down(l)[l->knum]);
			l->knum--;
			move_kv(p, pp - 1, l, l->knum);
			return;
		}
		else {//if (pp < p->knum && (r = down(p)[pp + 1])->knum > B){
			move_rdata(x, pos + 1, B, -1);
			move_kv(x, B - 1, p, pp);
			move_kv(p, pp, r, 0);
			set_down(x, B, down(r)[0]);
			set_down(r, 0, down(r)[1]);
			move_rdata(r, 1, r->knum, -1);
			r->knum--;
			return;
		}
	}
	if (x == root && x->knum == 1){
		root = down(x)[0];
		root->setp(0, 0);
		free_node(x);
	}
	else { //normal case: into->knum < 2*B;
		move_rdata(x, pos + 1, x->knum, -1);
		x->knum--;
	}
}

