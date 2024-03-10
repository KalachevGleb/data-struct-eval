#ifndef BTREET_H
#define BTREET_H
#include "alloc.h"
#include <iostream>

template<class K, class V, unsigned int B, class Cmp>
class BTreeTBase{//B-tree template base for movable keys
    static_assert(B > 1, "B must be greater than 1");
    static_assert(std::is_trivially_move_constructible_v<K> && std::is_trivially_move_assignable_v<V>, "K must be trivially movable");
    static_assert(std::is_trivially_move_constructible_v<V> && std::is_trivially_move_assignable_v<V>, "V must be trivially movable");
protected:
	struct _k{
		char k[sizeof(K)];
		operator const K&()const{ return *(const K*)k; }
	};
	struct _v{
		char v[sizeof(V)];
		operator const V&()const{ return *(const V*)v; }
	};

	bool check_l, check_r;
	size_t _sz, h;

	//char *mbuf; //size of this buf must be f_size;
	struct BNode;
	struct BLeaf{
		BNode *p;
		struct{
			unsigned int pnum : 16;
			unsigned int knum : 15;
			unsigned int is_leaf : 1;
		};
		_k keys[2 * B];
		_v vals[2 * B];
		void setp(BNode *n, int pn){
			pnum = pn; p = n;
		}
		void move_kv(int p1, BLeaf *s, int p2, int len){
			Memcpy(keys + p1, s->keys + p2, len);
			Memcpy(vals + p1, s->vals + p2, len);
		}
		BLeaf(){ is_leaf = true; }
	};
	struct BNode : public BLeaf{
		union{
			BNode *dn[2 * B + 1];
			BLeaf *dl[2 * B + 1];
		};
		BNode(){ this->is_leaf = false; }
	};
	struct _findres{
		union{
			BLeaf * l;
			BNode * n;
		};
		struct{
			unsigned int pos : 16;
			unsigned int is_eq : 1;
		};
        _findres():n(nullptr), pos(0), is_eq(0){}
		_findres(BNode *nn, int cc, bool eq){ n = nn; pos = cc; is_eq = eq ? 1 : 0; }
		_findres(BLeaf *nn, int cc, bool eq){ l = nn; pos = cc; is_eq = eq ? 1 : 0; }
	};

	AllocRef<BLeaf> _al_l;
	AllocRef<BNode> _al_n;
	//int node_size()const{ return int((char*)(&((BNode*)0)->data[0]) - (char*)0) + int(f_size*_2B+sizeof(void*)); }
	//int leaf_size()const{ return int((char*)(&((BNode*)0)->data[0]) - (char*)0) + key_size*_2B; }
	//void move_node_key
	/* sets value d to the n-th child pointer of l */
	void set_down(BNode *l, int n, BLeaf *d)const{
		l->dl[n] = d; d->setp(l, n);
	}
	void move_kv(BLeaf *l, int p1, BLeaf *r, int p2){
		l->keys[p1] = r->keys[p2];
		l->vals[p1] = r->vals[p2];
	}
	void set_kv(BLeaf *l, int p1, _k *k, _v *v){
		l->keys[p1] = *k;
		l->vals[p1] = *v;
	}
	/*function moves the sequence <(end-start) times (key, value)>
	by d places right starting from key with number start.
	If d is negative then function moves the sequence left*/
	void moveleft_leafdata(BLeaf *l, int start, int end){
		for (int i = start; i < end; i++){
			l->keys[i - 1] = l->keys[i];
			l->vals[i - 1] = l->vals[i];
		}
	}
	void moveright_leafdata(BLeaf *l, int start, int end){
		for (int i = end; i > start; i--){
			l->keys[i] = l->keys[i - 1];
			l->vals[i] = l->vals[i - 1];
		}
	}
	/*function moves the sequence <(end-start) times (child pointer, key, value)>
	by d places right starting from child pointer with number start.
	If d is negative then function moves the sequence left*/
	template<int s>
	void moveleft_sdata(BNode *l, int start, int end){
		BLeaf **src = l->dl;
		for (int i = start + s; i <= end; i++)
			(src[i - 1] = src[i])->pnum--;
		moveleft_leafdata(l, start, end);
	}
	template<int s>
	void moveright_sdata(BNode *l, int start, int end){
		BLeaf **src = l->dl;
		for (int i = end; i >= start + s; i--)
			(src[i+1] = src[i])->pnum++;
		moveright_leafdata(l, start, end);
	}
	/* setting correct pointers p for all children with
	numbers from start to end */
	void set_ptrs(BNode *l, int start, int end){
		BLeaf **src = l->dl;
		for (int i = start; i < end; i++)
			src[i]->setp(l, i);
	}
	/* setting correct pointers p for all children with
	numbers from start (by default for all) */
	void set_ptrs(BNode *l, int start = 0){ return set_ptrs(l, start, l->knum + 1); }
	
	// the root of B-tree
	union {
		BLeaf *l;
		BNode *n;
	} root;
	//inserts pair (k,v) into leaf l at position pos;
	_findres _insert_leaf(BLeaf *into, _k *k, _v *v, unsigned int pos){
		//	memcpy(mbuf, k, key_size);
		++_sz;
		if (into){
			if (into->knum == 2*B){
				BLeaf *l, *r; BNode *p = into->p;
				unsigned int pp = into->pnum;
				if (p){
					if (check_l && pp && (l = p->dl[pp - 1])->knum < 2*B){
						move_kv(l, l->knum, p, pp - 1);
						l->knum++;
						if (!pos){
							set_kv(p, pp - 1, k, v);
							return _findres(p, pp - 1, true);
						}
						else {
							move_kv(p, pp - 1, into, 0);
							moveleft_leafdata(into, 1, pos);
							set_kv(into, pos - 1, k, v);
							return _findres(into, pos - 1, true);
						}
					}
					else if (check_r && pp < p->knum && (r = p->dn[pp + 1])->knum < 2*B){
						moveright_leafdata(r, 0, r->knum);
						r->knum++;
						move_kv(r, 0, p, pp);
						if (pos == 2*B){
							set_kv(p, pp, k, v);
							return _findres(p, pp, true);
						}
						else{
							move_kv(p, pp, into, 2*B - 1);
							moveright_leafdata(into, pos, 2*B - 1);
							set_kv(into, pos, k, v);
							return _findres(into, pos, true);
						}
					}
				}
				BLeaf *n = _al_l.get_new();
				n->knum = into->knum = B;
				n->p = p;
				_findres res(into, pos, true);
				if (pos <= B){
					n->move_kv(0, into, B, B);
					moveright_leafdata(into, pos, B);
					set_kv(into, pos, k, v);
				}
				else{ // pos >= B+1
					int p1 = pos - B - 1;
					n->move_kv(0, into, B + 1, p1);
					set_kv(n, p1, k, v);
					n->move_kv(p1 + 1, into, pos, 2 * B - pos);
					res.l = n;
					res.pos = p1;
				}
				_insert(p, into->keys+B, into->vals+B, pp, n);
				if (pos == B){
					for (res.n = into->p, res.pos = into->pnum; res.n->knum <= res.pos;){
						res.pos = res.n->pnum; res.n = res.n->p;
					}
				}
				return res;
			}
			else { //into->knum < 2*B;
				moveright_leafdata(into, pos, into->knum);
				set_kv(into, pos, k, v);
				into->knum++;
				return _findres(into, pos, true);
			}
		}
		else{ // into = 0 => root = 0
			root.l = _al_l.get_new();
			root.l->knum = 1;
			root.l->setp(0, 0);
			set_kv(root.l, 0, k, v);
			h = 1;
			return _findres(root.l, 0, true);
		}
	}
	//balancing procedure for insert
	void _insert(BNode *into, _k *k, _v *v, unsigned int pos, BLeaf *y){
		while (into && into->knum == 2*B){
			BNode *l, *r, *p = into->p;
			unsigned int pp = into->pnum;
			if (p){
				if (check_l && pp && (l = p->dn[pp - 1])->knum < 2*B){
					move_kv(l, l->knum, p, pp - 1);
					l->knum++;
					set_down(l, l->knum, into->dn[0]);
					if (!pos){
						set_kv(p, pp - 1, k, v);
					}
					else {
						move_kv(p, pp - 1, into, 0);
						moveleft_sdata<0>(into, 1, pos);
						set_kv(into, pos - 1, k, v);
					}
					set_down(into, pos, y);
					return;
				}
				else if (check_r && pp < p->knum && (r = p->dn[pp + 1])->knum < 2*B){
					moveright_sdata<0>(r, 0, r->knum);
					r->knum++;
					move_kv(r, 0, p, pp);
					if (pos == 2*B){
						set_kv(p, pp, k, v);
						set_down(r, 0, y);
					}
					else{
						move_kv(p, pp, into, 2*B - 1);
						set_down(r, 0, into->dn[2*B]);
						moveright_sdata<1>(into, pos, 2*B - 1);
						set_kv(into, pos, k, v);
						set_down(into, pos + 1, y);
					}
					return;
				}
			}
			BNode *n = _al_n.get_new();
			n->knum = into->knum = B;
			n->p = p;
			if (pos < B){
				n->move_kv(0, into, B, B);
				Memcpy(n->dn, into->dn + B, B + 1);
				moveright_sdata<1>(into, pos, B);
				set_kv(into, pos, k, v);
				set_down(into, pos + 1, y);
			}
			else if (pos == B){
				n->move_kv(0, into, B, B);
				Memcpy(n->dn + 1, into->dn + B + 1, B);
				set_kv(into, pos, k, v);
				set_down(n, 0, y);
			}
			else{ // pos >= B+1
				int p1 = pos - B - 1;
				n->move_kv(0, into, B + 1, p1);
				Memcpy(n->dn, into->dn + B + 1, p1 + 1);
				set_kv(n, p1, k, v);
				n->dl[p1 + 1] = y;
				n->move_kv(p1 + 1, into, pos, 2 * B - pos);
				Memcpy(n->dn + p1 + 2, into->dn + pos + 1, 2 * B - pos);
			}
			set_ptrs(n);
			k = into->keys + B;
			v = into->vals + B;
			into = p; pos = pp; y = n;
		}
		if (!into){ //root was splitted
			into = _al_n.get_new();
			into->knum = 1;
			into->setp(0, 0);
			set_down(into, 0, root.l);
			set_down(into, 1, y);
			set_kv(into, 0, k, v);
			root.n = into;
			h++;
		}
		else { //normal case: into->knum < 2*B;
			moveright_sdata<1>(into, pos, into->knum);
			set_kv(into, pos, k, v);
			set_down(into, pos + 1, y);
			into->knum++;
		}
	}
	//removes k-th record from leaf l
	_findres _remove_leaf(BLeaf *x, unsigned int pos){
		--_sz;
		if (x == root.l && x->knum == 1){
			_al_l.free(x); root.l = 0;
			h = 0;
			return _findres();
		}
		else if (x != root.l && x->knum == B){
			BLeaf *l = 0, *r = 0;
			BNode *p = x->p; //x != root => x->p != 0
			unsigned int pp = x->pnum;
			if (pp && (l = p->dn[pp - 1])->knum == B){
				move_kv(l, l->knum, p, pp - 1);
				l->move_kv(l->knum + 1, x, 0, pos);
				int p1 = l->knum + 1 + pos;
				l->move_kv(p1, x, pos + 1, B - pos - 1);
				l->knum += B;
				_al_l.free(x);
				_remove(p, pp - 1);
				return _findres(l, p1, true);
			}
			else if (pp < p->knum && (r = p->dn[pp + 1])->knum == B){
				moveleft_leafdata(x, pos + 1, B);
				move_kv(x, B - 1, p, pp);
				x->move_kv(B, r, 0, r->knum);
				x->knum += r->knum;
				_al_l.free(r);
				_remove(p, pp);
				return _findres(x, pos, true);
			}
			else if (pp){// && (l = p->dn[pp - 1])->knum > B){
				moveright_leafdata(x, 0, pos);
				l->knum--;
				move_kv(x, 0, p, pp - 1);
				move_kv(p, pp - 1, l, l->knum);
				return _findres(x, pos + 1, true);
			}
			else {// if (pp < p->knum && (r = p->dn[pp + 1])->knum > B){
				moveleft_leafdata(x, pos + 1, B);
				move_kv(x, B - 1, p, pp);
				move_kv(p, pp, r, 0);
				moveleft_leafdata(r, 1, r->knum);
				r->knum--;
				return _findres(x, pos, true);
			}
		}
		else{	//normal case: into->knum < 2*B;
			moveleft_leafdata(x, pos + 1, x->knum);
			x->knum--;
			return _findres(x, pos, true);
		}
	}
	//balancing procedure for remove
	void _remove(BNode *x, unsigned int pos){
		while (x != root.n && x->knum == B){
			BNode *l = 0, *r = 0, *p = x->p; //x != root => x->p != 0
			unsigned int pp = x->pnum;
			if (pp && (l = p->dn[pp - 1])->knum == B){
				move_kv(l, l->knum, p, pp - 1);
				l->move_kv(l->knum + 1, x, 0, pos);
				Memcpy(l->dn + l->knum + 1, x->dn, pos + 1);
				int t = l->knum + 1 + pos, s = B - pos - 1;
				l->move_kv(t, x, pos + 1, s);
				Memcpy(l->dn + t + 1, x->dn + pos + 2, s);
				l->knum += B;
				set_ptrs(l, l->knum - B);
				_al_n.free(x);
				x = p; pos = pp - 1;
			}
			else if (pp < p->knum && (r = p->dn[pp + 1])->knum == B){
				moveleft_sdata<1>(x, pos + 1, B);
				move_kv(x, B - 1, p, pp);
				x->move_kv(B, r, 0, r->knum);
				Memcpy(x->dn + B, r->dn, r->knum + 1);
				x->knum += r->knum;
				set_ptrs(x, B);
				_al_n.free(r);
				x = p; pos = pp;
			}
			else if (pp){// && (l = p->dn[pp - 1])->knum > B){
				moveright_sdata<0>(x, 0, pos);
				move_kv(x, 0, p, pp - 1);
				set_down(x, 0, l->dn[l->knum]);
				l->knum--;
				move_kv(p, pp - 1, l, l->knum);
				return;
			}
			else {//if (pp < p->knum && (r = p->dn[pp + 1])->knum > B){
				moveleft_sdata<1>(x, pos + 1, B);
				move_kv(x, B - 1, p, pp);
				move_kv(p, pp, r, 0);
				set_down(x, B, r->dn[0]);
				moveleft_sdata<0>(r, 1, r->knum);
				r->knum--;
				return;
			}
		}
		if (x == root.n && x->knum == 1){
			root.l = x->dn[0];
			root.l->setp(0, 0);
			_al_n.free(x);
			h--;
		}
		else { //normal case: into->knum < 2*B;
			moveleft_sdata<1>(x, pos + 1, x->knum);
			x->knum--;
		}
	}
public:
	int size()const{return _sz;}
	int getB()const{ return B; }
	void setCheckLR(bool l, bool r){
		check_l = l; check_r = r;
	}
	BTreeTBase(){ check_l = check_r = true; root.l = 0; _sz = h = 0; }
	~BTreeTBase(){}

//	template<class T>
	struct _default_cmp{
		template<class T>
		bool less(const T&x, const T&y)const{
			return x<y;
		}
		template<class T>
		int compare(const T& x, const T&y)const{
			return x<y ? -1 : y<x ? 1 : 0;
		}
	};
	double fill_coef()const{
		double sp = 2*B*double(_al_n.count()+_al_l.count());
		return double(_sz)/sp;
	}
	size_t memory_usage()const{
		return _al_n.memory_usage() + _al_l.memory_usage();
	}
//	void _clear();
};

/* class Cmp must contain function:
 *    int compare(const K&x, const K&y)const;
 * or
 *    static int compare(const K&x, const K&y).
 *
 * To use other type K1 as argument of findT, insertT, removeT
 * must be defined functions
 *    template<class K1>
 *    int compare(const K&x, const K1&y)const;
 * or
 *    template<class K1>
 *    static int compare(const K&x, const K1&y).
 * in the other case each comparation will convert K1 to K.
 * These functions useful when constructor of class K allocates memory,
 * like std::string and it is ineffcient to convert char* to std::string
 * before each find call.
 * K must be a movable type
 */

template<class K, class V, int B, class Cmp = BTreeBase::_default_cmp>
class BTreeT : public BTreeTBase<K,V,B,Cmp>{
	typedef typename BTreeTBase<K,V,B,Cmp>::BNode _node;
    using _v = typename BTreeTBase<K,V,B,Cmp>::_v;
    using _k = typename BTreeTBase<K,V,B,Cmp>::_k;
    using _findres = typename BTreeTBase<K,V,B,Cmp>::_findres;
	Cmp _cmp;
	template<class K1>
	_findres _findins(const K1 &key)const{
		_node *n = this->root.n;
		int c = 0;
		if (!n)return _findres((_node*)0, 0, false);
		int b, e, m;
		size_t i;
		for (i = 1; ; i++){
			for (b = 0, e = n->knum; b < e;){
				m = (b + e) >> 1;
				c = _cmp.compare(n->keys[m], key);
				if (c < 0)b = m + 1;
				else if (c>0)e = m;
				else return _findres(n, m, true);
			}
			if (i == this->h)break;
			n = n->dn[b];
		}
		return _findres(n, b, false);
	}
	_findres _findins_l(const K &key) const{
		_node *n = this->root.n, *n0 = 0;
		if (!n)return _findres((_node*)0, 0, false);
		unsigned int b, e, m, b0=0;
		size_t i;
		for (i = 1; ; i++){
			for (b = 0, e = n->knum; b < e;){
				m = (b + e) >> 1;
				if(_cmp.less(n->keys[m], key))b = m + 1;
				else e = m;
			}
			if (b < n->knum){
				n0 = n; b0 = b;
			}
			if (i == this->h)break;
			n = n->dn[b];
		}
		if (!n0 || _cmp.less(key, n0->keys[b0]))return _findres(n, b, false);
		return _findres(n0, b0, true);
	}
	void _delrec(_node *n, int d = 1){
		if (d == this->h){
			for (unsigned int i = 0; i < n->knum; i++){
				_destruct((K*)(n->keys + i));
				_destruct((V*)(n->vals + i));
			}
			this->_al_l.free(n);
		}
		else{
			this->_delrec(n->dn[0], d + 1);
			for (unsigned int i = 0; i < n->knum; i++){
				_destruct((K*)(n->keys + i));
				_destruct((V*)(n->vals + i));
				this->_delrec(n->dn[i + 1], d + 1);
			}
			this->_al_n.free(n);
		}
	}
	void print_rec(_node *n, int rec){
		if (rec==this->h){
			for (unsigned int i = 0; i < n->knum; i++){
				for (int j = 0; j < rec; j++)std::cout << "    ";
				std::cout << n->keys[i] << "   " << n->vals[i] << "\n";
			}
		}
		else{
			print_rec(n->dn[0], rec + 1);
			for (unsigned int i = 0; i < n->knum; i++){
				for (int j = 0; j < rec; j++)std::cout << "    ";
				std::cout << n->keys[i] << "   " << n->vals[i] << "\n";
				print_rec(n->dn[i + 1], rec + 1);
			}
		}
	}
public:
	void print(){
		if (!this->root.l)std::cout << "<empty B-tree>\n";
		else{
			std::cout << "============================\n";
			print_rec(this->root.n, 1);
			std::cout << "============================\n\n";
		}
	}
	class const_iterator{
	//	const BTreeT<K, V, B, Cmp> *_t;
		_node *_n;
		unsigned int _pos;
		const_iterator(_findres r){ _n = r.n; _pos = r.pos; }
		void _go_up(){
			while (_pos >= _n->knum && _n->p){
				_pos = _n->pnum;
				_n = _n->p;
			}
		}
		friend class BTreeT<K, V, B, Cmp>;
	public:
		const_iterator(){ _n = 0;}
		const_iterator& to_start(){
			if (!_n)return *this;
			while (_n->p)_n = _n->p;
			while (!_n->is_leaf)
				_n = this->_t->down(_n)[0];
			_pos = 0;
			return *this;
		}
		const_iterator& to_end(){
			if (!_n)return *this;
			while (_n->p)_n = _n->p;
			_pos = _n->knum;
			return *this;
		}

		bool isValid()const{ return _n && _pos < _n->knum; }
		const K& key()const{ return this->_t->keys[this->_pos]; }
		const K* operator->()const{ return (const K*)(this->_t->keys + _pos); }
		const_iterator& operator++(){
			if (_n->is_leaf){
				if (++_pos < _n->knum)return *this;
				while (_pos >= _n->knum){
					if (!_n->p) return *this;
					_pos = _n->pnum;
					_n = _n->p;
				}
				return *this;
			}
			_n = _n->dn[_pos + 1];
			while (!_n->is_leaf){
				_n = _n->dn[0];
			}
			_pos = 0;
			return *this;
		}
		const_iterator& operator--(){
			if (_n->is_leaf){
				//if (--_pos < 0)return *this;
				while (!_pos){
					if (!_n->p){ _pos = _n->knum; return *this; } // go to end
					_pos = _n->pnum;
					_n = _n->p;
				}
				_pos--;
				return *this;
			}
			_n = _n->dn[_pos];
			while (!_n->is_leaf){
				_n = _n->dn[_n->knum];
			}
			_pos = _n->knum - 1;
			return *this;
		}
		const_iterator operator++(int){
			const_iterator it(*this);
			++*this;
			return it;
		}
		const_iterator operator--(int){
			const_iterator it(*this);
			--*this;
			return it;
		}
		bool operator == (const_iterator it)const{ return _n == it._n && _pos == it._pos; }
		bool operator != (const_iterator it)const{ return _n != it._n || _pos != it._pos; }
	};

	BTreeT(){}
	~BTreeT(){ clear(); }
	void clear(){
		if (this->root.n)this->_delrec(this->root.n);
		this->root.n = 0;
		this->_sz = this->h = 0;
	}
private:
	template<class K1, class V1>
	const_iterator _insertT(const _findres &fr, const K1& key, const V1 &val, bool replace){
		if(fr.is_eq){
			if (replace)
				*(K*)(fr.n->keys + fr.pos) = key;
			return fr;
		}
		_k sk;  new(&sk)K(key);
		_v sv;  new(&sv)V(val);
		return this->_insert_leaf(fr.n, &sk, &sv, fr.pos);
	}
public:
	template<class K1, class V1>
	const_iterator insertT(const K1& key, const V1 &val, bool replace = true){
		return this->_insertT(_findins(key), key, val, replace);
	}
	template<class V1>
	const_iterator insertL(const K& key, const V1 &val, bool replace = true){
		return this->_insertT(_findins_l(key), key, val, replace);
	}
	const_iterator erase(const_iterator it){
		if (it.isValid()){
			const_iterator i = it;
			_destruct((K*)(i._n->keys + i._pos));
			_destruct((V*)(i._n->vals + i._pos));
			if (!i._n->is_leaf){//then it._n is leaf
				++it;
				this->move_kv(i._n, i._pos, it._n, it._pos);
			}
			return this->_remove_leaf(it._n, it._pos);
		}
		return it;
	}
	template<class K1>
	const_iterator findT(const K1 &key)const{
		_findres r = _findins(key);
		if (r.is_eq)return r;
		return end();
	}
	template<class K1>
	bool removeT(const K1& key){
		const_iterator n = findT(key);
		return n != erase(n);
	}
	bool removeL(const K& key){
		const_iterator n = findL(key);
		return n != erase(n);
	}
	template<class K1>
	const_iterator findRightT(const K1 &key)const{
		const_iterator r(_findins(key));
		r._go_up();
		return r;
	}
	template<class K1>
	const_iterator findLeftT(const K1 &key)const{
		_findres r = _findins(key);
		const_iterator it(r);
		if (r.is_eq)return it;
		return --it;
	}
	const_iterator findRightL(const K &key)const{
		const_iterator r(_findins_l(key));
		r._go_up();
		return r;
	}
	const_iterator findL(const K &key)const{
		_findres r = _findins_l(key);
		if (r.is_eq)return r;
		return end();
	}
	const_iterator findLeftL(const K &key)const{
		_findres r = _findins_l(key);
		const_iterator it(r);
		if (r.is_eq)return it;
		return --it;
	}
	const_iterator findRight(const K &key)const{
		return findRightT(key);
	}
	const_iterator findLeft(const K &key)const{
		return findLeftT(key);
	}
	const_iterator find(const K &key) const{
		//	return _find(key);
		return findT(key);
	}
	template<class K1>
	bool containsT(const K1 &key){
		return findT(key).isValid();
	}
	bool containsL(const K &key){
		return findL(key).isValid();
	}
	bool contains(const K&key)const{
		return findT(key).isValid();
	}
	const_iterator insert(const K& key, const V& val, bool replace = true){
		return insertT(key, val, replace);
	}
	/*	template<class K1>
	BTree<K, Cmp>& operator <<= insert(const K& key){
	insertT(key, replace);
	return *this;
	}*/
	bool remove(const K &key){ return removeT(key); }
/*	BTree<K, Cmp>& operator<<(const K& key){
		insertT(key, replace);
		return *this;
	}
	BTree<K, Cmp>& operator-=(const K& key){
		remove(key);
		return *this;
	}*/
	const_iterator begin()const{
		const_iterator it;
		it._n = this->root.n;
		return it.to_start();
	}
	const_iterator end()const{
		if (!this->_sz)return const_iterator();
		const_iterator it;
		it._n = this->root.n; it._pos = this->root.n->knum;
		return it;
	}
};

#endif // BTREET_H
