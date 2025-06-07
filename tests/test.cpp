//#define USE_ALLOCATOR
#include "stringmap.h"
#include "pst.h"
#include "rbtree.h"
#include "avltree.h"
#include "sort.h"
#include "23Tree.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <map>
#include <cstdio>
#include <string>
#include <vector>
#include "Btree.h"
#include "BtreeT.h"
#include <random>
#include <chrono>

class Timer {
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastCheck;
public:
    Timer() {
        reset();
    }
    double getTime(bool reset = false) {
        auto current = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = current - _lastCheck;
        if (reset) _lastCheck = current;
        return elapsed.count();
    }
    void reset() {
        getTime(true);
    }
};

const char* toStr8(int i){
	static char str[]="                                ";
	int j;
	if(!i)return "0";
	for(j=32; i; i>>=3)
		str[--j] = '0'+(i&7);
	return str+j;
}
const char* toStr4(int i){
	static char str[]="                                ";
	int j;
	if(!i)return "0";
	for(j=32; i; i>>=2)
		str[--j] = '0'+(i&3);
	return str+j;
}
const char* toStr2(int i){
	static char str[]="                                ";
	int j;
	if(!i)return "0";
	for(j=32; i; i>>=1)
		str[--j] = '0'+(i&1);
	return str+j;
}
//static int rnd = 0;
std::mt19937 rnd;
struct Point{
	//union{
//	struct pt{
	int x, y;// , z[30];
//	} *p;
	//	long long ll;
	//};
	Point(int a, int b){x = a; y = b; }
	Point(){}
	bool operator < (const Point &p2)const{return x<p2.x || (x == p2.x && y<p2.y);}
    bool operator == (const Point &p2)const{return x == p2.x && y == p2.y;}
};

struct HashPoint{
    size_t operator()(const Point &p)const{
        return std::hash<uint64_t>()((uint64_t)p.x<<32 | p.y);
    }
};

struct PointP{
	//union{
	Point *p;
	//	long long ll;
	//};
	PointP(int a, int b){ p = new Point(a,b); }
	PointP(const PointP&ptx){ p = new Point(*ptx.p); }
    PointP(PointP&&ptx){
        p = ptx.p;
        ptx.p = nullptr;
    }
    PointP& operator =(PointP&&ptx){
        std::swap(p, ptx.p);
        return *this;
    }

	explicit PointP(const Point&ptx){
        p = new Point(ptx);
    }
	PointP& operator=(const PointP&ptx){ *p = *ptx.p; return *this; }
	PointP& operator=(const Point&ptx){ *p = ptx; return *this; }
	PointP(){ p = new Point; }
	~PointP(){ delete p; }
	bool operator < (const PointP &p2)const{ return *p<*p2.p; }
};
struct PointI{
	//union{
	Point p;
	int i;
	//	long long ll;
	//};
	PointI(int a, int b, int ii) :p(a, b){ i = ii; }
	explicit PointI(const Point&ptx){ p = ptx; }
	PointI& operator=(const Point&ptx){ p = ptx; return *this; }
	bool operator < (const PointI &p2)const{ return p<p2.p; }
};
struct point_cmp{
	static size_t nl, ncmp, nhash, neq;
	bool operator()(const Point &p1, const Point &p2)const{//for std::map
		nl++;
		return p1.x<p2.x || (p1.x == p2.x && p1.y<p2.y);
	}
	bool operator()(const PointP &p1, const PointP &p2)const{//for std::map
		nl++;
		return p1<p2;
	}
	bool operator()(const PointI &p1, const PointI &p2)const{//for std::map
		nl++;
		return p1.p<p2.p;
	}
	static void reset(){ nl = ncmp = nhash = neq = 0; }
	static void print(){
		if(nl)  printf("\t%9lld operations less   ", (long long)nl);
		if(ncmp)printf("\t%9lld operations compare", (long long)ncmp);
        if(nhash)printf("\t%9lld operations hash   ", (long long)nhash);
        if(neq) printf("\t%9lld operations equal  ", (long long)neq);
		printf("\n");
	}
	bool less(const Point& p1, const Point& p2)const{
		nl++;
		return p1.x<p2.x || (p1.x == p2.x && p1.y<p2.y);
    }
	bool less(const PointP& p1, const PointP& p2)const{
		nl++;
		return p1<p2;
	}
	bool less(const PointI& p1, const PointI& p2)const{
		nl++;
		return p1.p<p2.p;
	}
	bool equal(Point p1, Point p2)const{
        neq++;
        return p1.x == p2.x && p1.y == p2.y;
    }
	bool equal(PointP p1, PointP p2)const{
        neq++;
        return p1.p->x == p2.p->x && p1.p->y == p2.p->y;
    }
	bool equal(PointI p1, PointI p2)const{
        neq++;
        return p1.p.x == p2.p.x && p1.p.y == p2.p.y;
    }
	static int compare(const Point& p1, const Point& p2){
		ncmp++;
		return p1.x<p2.x ? -1 : p1.x>p2.x ? 1 : p1.y<p2.y ? -1 : p1.y>p2.y ? 1 : 0;
	}
	static int compare(const PointP& p1, const PointP& p2){
		ncmp++;
		return p1.p->x<p2.p->x ? -1 : p1.p->x>p2.p->x ? 1 : p1.p->y<p2.p->y ? -1 : p1.p->y>p2.p->y ? 1 : 0;
	}
	static int compare(const PointP& p1, const Point& p2){
		ncmp++;
		return p1.p->x < p2.x ? -1 : p1.p->x>p2.x ? 1 : p1.p->y<p2.y ? -1 : p1.p->y>p2.y ? 1 : 0;
	}
	static int compare(const PointI& p1, const PointI& p2){
		ncmp++;
		return compare(p1.p, p2.p);
	}
	static int compare(const PointI& p1, const Point& p2){
		ncmp++;
		return compare(p1.p, p2);
	}
	static int cmp(const void*p1, const void* p2){
		return compare(*(Point*)p1, *(Point*)p2);
	}
    struct Hash {
        size_t operator()(const Point& p)const{
            nhash++;
            return std::hash<uint64_t>()((uint64_t)p.x<<32 | p.y);
        }
    };
    struct Eq {
        bool operator()(const Point &p1, const Point &p2) const {
            neq++;
            return p1.x == p2.x && p1.y == p2.y;
        }
    };
};
using PointHash = std::unordered_map<Point, int, point_cmp::Hash, point_cmp::Eq>;
struct point_cmp_more{
	bool operator()(Point p1, Point p2)const{//for std::map
		point_cmp::nl++; return p1.x>p2.x || (p1.x == p2.x && p1.y>p2.y);
	}
};
size_t point_cmp::nl = 0;
size_t point_cmp::ncmp = 0;
size_t point_cmp::nhash = 0;
size_t point_cmp::neq = 0;

#define MAX_INT ((int)((unsigned int)(-1)>>1))
Point find(std::vector<Point> &v, int l, int r, int b){
	size_t i, n = v.size(); Point *data = &v[0], res(MAX_INT,MAX_INT);
	for(i=0; i<n; i++)
		if(data[i].x<=r && data[i].x>=l &&data[i].y<=b){
			if(res.y>data[i].y)res = data[i];
//			res++;
		}
	return res;
}
void find(std::vector<Point> &v, std::vector<int> &res, int l, int r, int b, int f){
	size_t i, n = v.size(); Point *data = &v[0];
	for(i=f; i<n; i++)
		if(data[i].x<=r && data[i].x>=l && data[i].y<=b){
			res.push_back((int)i);
		}
}

template<class T>
bool operator!=(const std::vector<T> &x, const std::vector<T> &y){
	if(x.size()!=y.size())return true;
	for(int i=0, n=x.size(); i<n; i++)
		if(x[i]!=y[i])return true;
	return false;
}
void print(const std::vector<int> &v, const std::vector<Point> &d){
	printf("(");
	for(int i=0, n=(int)v.size(); i<n; i++)
		printf("[%d, %d]%s", d[v[i]].x,d[v[i]].y, i<n-1 ? ", " : "");
	printf(")");
}
struct counter{
	int n;
	counter(){n=0;}
	void reset(){n=0;}
	void add(int){n++;}
};

template<class T, class T1, class Cmp>
bool searchT(const T* b, int sz, const T1&v, const Cmp & cmp){
	for (;;){
		if (!sz)return false;
		int m = sz / 2;
		int c = cmp.compare(b[m],v);
		if (!c)return true;
		if (c<0){ b += m + 1; sz -= m + 1; }
		else sz = m;
	}
	return false;
	(void)cmp;
}
template<class T, class Cmp>
bool search(const T* b, int sz, const T&v, const Cmp & cmp){
	return searchT(b, sz, v, cmp);
}

struct int_less{
	static long long nc;
	bool operator()(int x, int y)const{
        nc++;
	    return x<y;
    }
	bool operator()(double x, double y)const{
        nc++;
	    return x<y;
    }
};
struct int_greater{
	static long long nc;
	bool operator()(int x, int y)const{
        nc++;
	    return x>y;
    }
	bool operator()(double x, double y)const{
        nc++;
	    return x>y;
    }
};

long long int_less::nc = 0;
long long int_greater::nc = 0;

std::ostream& operator<<(std::ostream &s, Point p){
	return s << "(" << p.x << ", " << p.y << ")";
}

void testIntSort(int /*n*/){
	//double t;
	for(int k=20; k>=0; k--){
                sort_help::compare_count = 0;
		int N=100000000>>k;
		std::vector<double> A(N), sd;
		printf("N=%d\n",N);
		A[0]=12345;
		for(int i=1;i<N;i++) A[i]=(int(A[i-1])*199973+1);
		sd = A;
		int_greater::nc = 0;
		double t=clock();
                sort_help::m_sort(&sd[0], N, int_greater());
		t = (clock()-t)/CLOCKS_PER_SEC;
//		bool q=true;
//		for(int i=1;i<N;i++) if(sd[i]<sd[i-1]) q=false;
		printf("msort:        ncmp =%11lld, time = %6.3lf\n", int_greater::nc, t);

		//A[0]=12345;
	//	t1=clock();

		//qsort(A,N,sizeof(int),icmp);
	
		int_less::nc=0;
		sd = A;
		t = clock();
		quick_sort(&sd[0], sd.size(), int_less());
		t = (clock()-t)/CLOCKS_PER_SEC;
		printf("quick sort:   ncmp =%11lld, time = %6.3lf\n", int_less::nc, t);

	/*	int_less::nc=0;
		sd = A;
		{
			std::vector<double> tmp(N);
			t = clock();
			merge_sort(&sd[0], &tmp[0], sd.size(), int_less());
		//	check(&sd[0], sd.size(), int_less());
		}
		t = (clock()-t)/CLOCKS_PER_SEC;
		printf("merge sort:   ncmp =%11I64d, time = %6.3lf\n", int_less::nc, t);
	*/
		int_less::nc=0;
		sd = A;
		t = clock();
		heap_sort(&sd[0], sd.size(), int_less());
		t = (clock()-t)/CLOCKS_PER_SEC;
		printf("heap sort:    ncmp =%11lld, time = %6.3lf\n", int_less::nc, t);
		//QuickI(A,N);
	//	t2=clock();
//		q=true;
//		for(int i=1;i<N;i++) if(sd[i]<sd[i-1]) q=false;
	}
}

template<class K, class T, int N, class Cmp>
std::string name(const BTreeT<K, T, N, Cmp> &){
	return "template B-tree (B = "+std::to_string(N)+")";
}
template<class K, class T, class Cmp>
std::string name(const BTreeV<K, T, Cmp> &t){
	return "B-tree (B = "+std::to_string(t.getB())+")";
}
template<class K, class T, class Cmp>
std::string name(const Tree23<K, T, Cmp> &){
	return "2-3 tree";
}
template<class K, class T, class Cmp>
std::string name(const RBT<K,T,Cmp> &){
	return "Red-Black tree";
}
template<class K, class T, class Cmp>
std::string name(const AVLTree<K, T, Cmp> &){
	return "AVL-tree";
}
template<class K, class T, class Cmp>
std::string name(const std::map<K, T, Cmp> &){
	return "std::map";
}
template<class K, class T, class Hash, class Eq>
std::string name(const std::unordered_map<K, T, Hash, Eq> &){
    return "std::unordered_map";
}
template<class T>
std::string name(const PST<T> &){
	return "Priority search tree";
}
volatile int unused = 0;
template<class Cont>
int test_cont(int n, Cont &c, const Point *ins, const Point *d){
	//====================================
	int i, j=0;
	long long m;
	point_cmp::reset();
	std::string nm = name(c);
	printf("\n         testing %s : \n", nm.c_str());
	Timer timer;
	for (i = 0; i < n; i++){
		c.insert(ins[i], i);
	}
	double t = timer.getTime();
	printf("insert :         time = %7.3lf\t", t);
	point_cmp::print();

	m = c.memory_usage();

	point_cmp::reset();
	timer.reset();
	for (i = 0; i < n; i++){
		if(c.find(Point(d[i].x, d[i].y))!=c.end()) j++;
	}
	t = timer.getTime();
	printf("identical find : time = %7.3lf\t", t);
	point_cmp::print();

	point_cmp::reset();
	timer.reset();
	for (i = 0; i < n; i++){
		if (c.findRight(Point(d[i].x + 1, d[i].y))!=c.end()) j++;
	}
	t = timer.getTime();
	printf("find right :     time = %7.3lf\t", t);
	point_cmp::print();
	//	}
	point_cmp::reset();
	timer.reset();
	for (i = 0; i < n; i++){
		c.remove(ins[i]);
	}
	t = timer.getTime();
	printf("remove :         time = %7.3lf\t", t);
	point_cmp::print();
	printf("memory usage :    %11lld bytes\n", m);
	//====================================
    unused = j;
	return j;
}

template<bool unordered, class Cont>
int test_map(int n, Cont &mp, const Point *ins, const Point *d){
    //printf("\n          testing std::map:\n");
    point_cmp::reset();
    Timer timer;
    for (int i = 0; i < n; i++) {
        mp[ins[i]] = i;
    }
    double t = timer.getTime();
    printf("insert :         time = %7.3lf\t", t);
    point_cmp::print();
    point_cmp::reset();
    timer.reset();
    int found = 0;
    for (int i = 0; i < n; i++) {
        if (mp.find(Point(d[i].x, d[i].y)) != mp.end())
            found++;
    }
    unused += found;
    t = timer.getTime();
    printf("identical find : time = %7.3lf\t", t);
    point_cmp::print();

    if constexpr(!unordered) {
        point_cmp::reset();
        timer.reset();
        for (int i = 0; i < n; i++) {
            if (mp.lower_bound(Point(d[i].x + 1, d[i].y)) != mp.end())
                found++;
        }
        unused += found;
        t = timer.getTime();
        printf("find left :      time = %7.3lf\t", t);
        point_cmp::print();
    }
    point_cmp::reset();
    timer.reset();
    for (int i = 0; i < n; i++) {
        mp.erase(ins[i]);
    }
    t = timer.getTime();
    printf("remove :         time = %7.3lf\t", t);
    point_cmp::print();
    return found;
}

void test_sort(std::vector<Point> &d){
    auto d0 = d;
    int n = static_cast<int>(d.size());
    printf("===================================\n\n");
    printf("          testing sort and binary search:\n");
    Timer timer;
    double t = 0;
    for(int i=0; i<1; i++) {
        d = d0;
        timer.reset();
        point_cmp::reset();
        std::sort(&d[0], &d[0] + n, point_cmp());
        t = timer.getTime();
        printf("std::sort :        time = %7.3lf\t", t);
        point_cmp::print();
    }
    //	*/

    d = d0;
    timer.reset();
    point_cmp::reset();
    qsort(&d[0], n, sizeof(Point), point_cmp::cmp);
    t = timer.getTime();
    printf("c qsort :          time = %7.3lf\t", t);
    point_cmp::print();

    //	sd = d;
    for(int i=0; i<1; i++) {
        d = d0;
        timer.reset();
        point_cmp::reset();
        quick_sort(d.data(), d.size(), point_cmp());
        t = timer.getTime();
        //	check(&sd[0], sd.size(),point_cmp());
        printf("quick sort :       time = %7.3lf\t", t);
        point_cmp::print();
    }

    d = d0;
    timer.reset();
    point_cmp::reset();
    std::make_heap(d.begin(), d.end(), point_cmp());
    std::sort_heap(d.begin(), d.end(), point_cmp());
    t = timer.getTime();
    printf("std heap sort :    time = %7.3lf\t", t);
    point_cmp::print();
    //	*/

    d = d0;
    timer.reset();
    point_cmp::reset();
    heap_sort(d.data(), d.size(),point_cmp());
    t = timer.getTime();
    printf("heap sort :        time = %7.3lf\t", t);
    point_cmp::print(); // */

    d = d0;
    timer.reset();
    point_cmp::reset();
    std::stable_sort(&d[0], &d[0] + n, point_cmp());
    t = timer.getTime();
    printf("std::stable_sort : time = %7.3lf\t", t);
    point_cmp::print();
    //	*/

    d = d0;
    timer.reset();
    point_cmp::reset();
    {
        std::vector<Point> tmp(n);
        merge_sort(&d[0], &tmp[0], d.size(), point_cmp());
    }
    t = timer.getTime();
    printf("merge sort :       time = %7.3lf\t", t);
    point_cmp::print();// */

    point_cmp::reset();
    timer.reset();
    for (int i = 0; i < n; i++) {
        search(&d[0], (int)d.size(), d0[i], point_cmp());
    }
    t = timer.getTime();
    printf("identical search:  time = %7.3lf\t", t);
    point_cmp::print();

    point_cmp::reset();
    timer.reset();
    for (int i = 0; i < n; i++) {
        search(&d[0], (int)d.size(), Point(d0[i].x + 1, d0[i].y), point_cmp());
    }
    t = timer.getTime();
    printf("nearest search :   time = %7.3lf\t", t);
    point_cmp::print();
    /*	sdp = dp;
    t = clock();
    point_cmp::reset();
    quick_sort(&sdp[0], sdp.size(), point_cmp());
    t = (clock() - t) / CLOCKS_PER_SEC;
    //	check(&sd[0], sd.size(),point_cmp());
    printf("quick sort with pointers :  time = %7.3lf ", t);
    point_cmp::print();

    point_cmp::reset();
    t = clock();
    for (i = 0; i<n; i++){
    searchT(&sdp[0], (int)sdp.size(), d[i], point_cmp());
    }
    t = (clock() - t) / CLOCKS_PER_SEC;
    printf("identical search (pointers): time = %7.3lf ", t);
    point_cmp::print();

    point_cmp::reset();
    t = clock();
    for (i = 0; i<n; i++){
    searchT(&sdp[0], (int)sdp.size(), Point(d[i].x + 1, d[i].y), point_cmp());
    }
    t = (clock() - t) / CLOCKS_PER_SEC;
    printf("nearest search (pointers):   time = %7.3lf ", t);
    point_cmp::print();
    */
}

void test_strtree(int n)
{
    StringTree<int> tr;
    std::map<std::string, int> st;
    int k = 0xFFFF;
    std::vector<int> t23(n), sm(n), v(n);
    std::vector<std::string> sarr(n);
    rnd.seed(0);
    for (int i = 0; i < n; i++) {
        auto r = rnd() & k;
        sarr[i] = toStr2(r);
        v[i] = r;
    }
    //rnd.seed(46912378);
    Timer timer;
    for (int i = 0; i < n; i++) {
        //int r = v[i];
        //printf("sz = %3d   ", tr.size());
        switch (rnd() % 10) {
            case 0:// printf("step %4d : insert %d\n", i, r);
                t23[i] = tr.insert(sarr[i].c_str(), v[i], false).nodeCreated ? 1 : 0;
                //	sm[i] = st.insert(std::make_pair(toStr4(r), r)).second ? 1 : 0;
                //if(t23[i]){tr.print(); printf("=====================\n\n");tr.check();}
                break;
            case 2:// printf("step %4d : remove %d\n", i, r);
                //	sm[i] = st.erase(toStr4(r)) ? 1 : 0;
                //	if(sm[i]){tr.print(); printf("=====================\n\n");}
                t23[i] = tr.remove(sarr[i].c_str()) ? 1 : 0;
                break;
            default:// printf("step %4d : find %d\n", i, r);
                t23[i] = tr.find(sarr[i].c_str()).isValid() ? 1 : 0;
                //	sm[i] = st.find(toStr4(r)) == st.end() ? 0 : 1;
                break;
        }
    }
    double t = timer.getTime();
    printf("StringTree : time = %.3lf\n", t);
    timer.reset();
    for (int i = 0; i < n; i++)
        tr.find(sarr[i].c_str());
    t = timer.getTime();
    printf("StringTree : search time = %.3lf\n", t);

    timer.reset();
    rnd.seed(0);
    for (int i = 0; i < n; i++) {
        //int r = rnd()&k;
        switch (rnd() % 10) {
            case 0:
                sm[i] = st.insert(std::make_pair(sarr[i], v[i])).second ? 1 : 0;
                break;
            case 2:
                sm[i] = st.erase(sarr[i]) ? 1 : 0;
                break;
            default:
                sm[i] = st.find(sarr[i]) == st.end() ? 0 : 1;
                break;
        }
    }
    t = timer.getTime();
    printf("Set : time = %.3lf\n", t);
    timer.reset();
    for (int i = 0; i < n; i++)
        st.find(sarr[i]);
    t = timer.getTime();
    printf("Set : search time = %.3lf\n", t);
    for (int i = 0; i < n; i++) {
        if (sm[i] != t23[i]) {
            printf("error in %d step\n", i);
            break;
        }// */
    }
}
void test_pst(int n, const Point *insdel) {
     PST<int> pst;
     Timer timer;
     printf("\n          testing PST:\n");
     for(int i=0; i<n; i++){
        pst.insert(insdel[i].x, insdel[i].y, i);
     }
     double t = timer.getTime();
     printf("insert :         time = %7.3lf\n", t);

     timer.reset();
     for(int i=0; i<n; i++){
        //		printf("remove (%d,%d)\n", d[i]&kl, (d[i]>>k)&kl);
        pst.remove(insdel[i].x, insdel[i].y);
     }
     t = timer.getTime();
     printf("remove :         time = %7.3lf\n", t);
     //====================================*/
}

int main(int argc, char *argv[]){
	//	testIntSort(0);
	//PST<int> pst;
    //PST<int>::ConstPointer miny;
	//printf("pst node size = %zu\n", sizeof(PST<int>::PSTNode));
	//printf("pst node base size = %zu\n", sizeof(PSTBase::PSTNodeBase));
	RBT<Point, int, point_cmp> rbt;
	AVLTree<Point, int, point_cmp> avl;
	Tree23<Point, int, point_cmp> t23;
	BTreeV<Point, int, point_cmp> btr;
	BTreeT<Point, int, 32, point_cmp> btt;
	//	BTree<PointP, point_cmp> btrp;
	//StringTree<int> tr;
	//std::map<std::string, int> st;
	std::map<Point, int, point_cmp> mp;
    PointHash ump;
	int n;
	bool wait = true;
	if (argc > 1){
        n = std::atoi(argv[1]);
		if (n <= 0){
            printf("incorrect parameter\n");
            return 0;
        }
	} else {
        printf("Error: no parameter\n");
        return 1;
    }
	int k = 16, kl = (1 << k) - 1;

	std::vector<Point> d(n), d0;// , sd;
	//	std::vector<PointP> dp(n), sdp; 
	printf("random initialization ...\n");
	for (int i = 0; i < n; i++){
		int r = rnd();
		d[i].x = r&kl;
        d[i].y = (r >> k)&kl;
		//	dp[i] = d[i];
	}
	//	sd = d;

	std::vector<Point> *insdel = &d0;
	d0 = d;
	for (int i = 0; i < n; i++){
		int j = i + rnd() % (n - i);
		std::swap(d[i], d[j]);
	}
    // test_sort(d);
    //return 0;
	//	sd.clear();
    //====================================
    for(auto ord : {false, true}) {
        if(ord){
            printf("===================================\n\n         testing ordered insert/delete\n");
        }
        // test_pst(n, insdel->data());

        //====================================
        test_cont(n, t23, insdel->data(), d.data());

        //====================================
        for (int jj = 2; jj <= 64; jj <<= 1) {
            btr.setB(std::max(jj, 1));// btrp.setB(1 << jj);
            point_cmp::reset();
            test_cont(n, btr, insdel->data(), d.data());
        }
        for (int jj = 2; jj <= 1; jj++) {
            //====================================
            if (jj == 1)
                printf("\n     testing B-tree with simplified insertion:    ");
            btt.setCheckLR(jj == 0, jj == 0);
            test_cont(n, btt, &(*insdel)[0], &d[0]);
        }
        //====================================
        /*	for (int jj = 0; jj <= 6; jj++){
            btrp.setB(1 << jj);
            //====================================
            point_cmp::reset();
            printf("\n          testing B-tree (B = %d) with pointers:\n", int(1 << jj));
            t = clock();
            for (i = 0; i < n; i++){
                //		std::cout << "insert " << (*insdel)[i] << " :\n";
                btrp.insertT((*insdel)[i]);
                //		btr.print();
            }
            t = (clock() - t) / CLOCKS_PER_SEC;
            printf("insert :         time = %7.3lf\t", t);
            point_cmp::print();
            //	if(!ord){
            point_cmp::reset();
            t = clock();
            for (i = 0; i < n; i++){
                if (btrp.findT(d[i]).isValid()) j++;
                //			if (btr.find((*insdel)[i]).isValid()) j++;
            }
            t = (clock() - t) / CLOCKS_PER_SEC;
            printf("identical find : time = %7.3lf\t", t, j);
            point_cmp::print();

            point_cmp::reset();
            t = clock();
            for (i = 0; i < n; i++){
                if (btrp.findRightT(Point(d[i].x + 1, d[i].y)).isValid()) j++;
                //			if (btr.find(Point((*insdel)[i].x + 1, (*insdel)[i].y)).isValid()) j++;
            }
            t = (clock() - t) / CLOCKS_PER_SEC;
            printf("find right :     time = %7.3lf\t", t, j);
            point_cmp::print();
            //	}
            point_cmp::reset();
            t = clock();
            //btr.print();
            for (i = 0; i < n; i++){
                //		std::cout << "remove " << (*insdel)[i] << " :\n";
                btrp.removeT((*insdel)[i]);
                //		btr.print();
            }
            t = (clock() - t) / CLOCKS_PER_SEC;
            printf("remove :         time = %7.3lf\t", t);
            point_cmp::print();
        //====================================
        }
        */
        //====================================
        test_cont(n, avl, &(*insdel)[0], &d[0]);
        test_cont(n, rbt, &(*insdel)[0], &d[0]);

        //====================================

        printf("\n          testing std::map:\n");
        test_map<false>(n, mp, &(*insdel)[0], &d[0]);
        printf("\n          testing std::unordered_map:\n");
        test_map<true>(n, ump, &(*insdel)[0], &d[0]);
        //==========================================================
        insdel = &d;
//	d = sd;
        if (!ord) {
            test_sort(d);
        }
    }
    //====================================
    // test_stringtree(n);

	return 0;
}
