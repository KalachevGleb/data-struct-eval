#pragma once
#include <cstdio>
#include <cstring>
#include <cstdlib>
//#include <cmath>
#include <cstdint>
#include <utility>

namespace _sort_help{
//---------------------------------------------------------------------------
inline int Min(int A, int B){return A>B ? B : A;}

//---------------------------------------------------------------------------
template<class T, class Less>
int merge(const T *a1,const T *a2, size_t n1, size_t n2, T *res, const Less &less)
{
	size_t i, j;
	for(i=j=0; (i<n1)&&(j<n2); res++){
		if(less(a1[i],a2[j])) *res=a1[i++];
		else                  *res=a2[j++];
	}
	if(i<n1)do *(res++) = a1[i++]; while(i<n1);
	else while(j<n2) *(res++) = a2[j++];
	return (int)(i+j);
}

//---------------------------------------------------------------------------
template<class T, class Less>
int merge_swap(T *a1, T *a2, size_t n1, size_t n2, T *res, const Less &less)
{
	size_t i, j;
	T tmp;
	for(i=j=0; (i<n1)&&(j<n2); res++){
		if(less(a1[i],a2[j])) std::swap(res,a1+(i++),tmp);
		else                  std::swap(res,a2+(j++),tmp);
	}
	if(i<n1)do std::swap(res++, a1+(i++),tmp); while(i<n1);
	else while(j<n2) std::swap(res++, a2+(j++));
	return (int)(i+j);
}
//---------------------------------------------------------------------------
template<class T, class Less>
void bubbleSort(T *a, size_t n, const Less &less){
	size_t i, j;
	for(i=1; i<n; i++){
		if(less(a[i],a[i-1])){
			auto tmp = std::move(a[i]);
			j = i-1;
			do a[j+1] = std::move(a[j]); while(j-- && less(tmp, a[j]));
			a[j+1] = std::move(tmp);
		}
	}
}
//---------------------------------------------------------------------------
template<class T, class Less>
void mergeSort64(T *a, const Less &less){
	T tmp[64];
	int i;
	for(i=0; i<64; i+=4)
		bubbleSort(a+i,4,less);//sort4(a+i,less);
	for(i=0; i<64; i+=8)
		merge(a+i,a+i+4,4,4,tmp+i,less);
	for(i=0; i<64; i+=16)
		merge(tmp+i,tmp+i+8,8,8,a+i,less);
	merge(a,a+16,16,16,tmp,less);
	merge(a+32,a+48,16,16,tmp+32,less);
	merge(tmp,tmp+32,32,32,a,less);
//	check(a, 64, less);
}
//---------------------------------------------------------------------------
template<class T, class Less>
void mergeSort32(T *a, const Less &less){
	T tmp[32];
	int i;
	for(i=0; i<32; i+=8)
		bubbleSort(a+i,8,less);//sort4(a+i,less);
	merge(a,a+8,8,8,tmp,less);
	merge(a+16,a+24,8,8,tmp+16,less);
	merge(tmp,tmp+16,16,16,a,less);
}

//---------------------------------------------------------------------------
template<class T, class Less>
size_t qSortSplit(T *a, size_t n, const Less &less){
	size_t i, j;
	T c, d = a[n-1];
//	if((!a)||(N<=0))return ILLEGAL_NUMBER;
	if(n==1)return 0;
	for(i=0, j=n-2; i<=j; i++){
		if(less(d, a[i])){
			while(j>i && less(d, a[j])) j--;
			if(j==i)break;
			c=a[j]; a[j]=a[i]; a[i]=c;
			j--;
		}
	}
	c=a[i]; a[i]=d; a[n-1]=c;
	return i;
}
//---------------------------------------------------------------------------
template <typename T, typename Less>
void UpdateHeap(T *p, size_t i, size_t n, T val, const Less &less){
	size_t l, j;
	for(l=i, j = (l<<1)+2; j<n; l=j, j=(l<<1)+2){
		if(less(p[j], p[j-1])) --j;
		p[l] = p[j];
	}
	if(j == n){
		p[l] = p[j-1];
		l = j-1;
	}
	for(j=(l-1)>>1; l>i && less(p[j], val); j=(l-1)>>1){
		p[l] = p[j];
		l = j;
	}
	p[l] = val;
}
//---------------------------------------------------------------------------
template<class T>
size_t bfind(const T *a, size_t N, const T &d){
	size_t i=0,j=N-1;
	if((!a) || (N<=0))
        return size_t(-1);
	for(auto c=(i+j)>>1; j-i>0; c=(i+j)>>1){
		if(d<=a[c])j=c;
		else i=c+1;
	}
	return i;
}
//---------------------------------------------------------------------------
template<class T, class Less>
size_t extractMax(T* a, size_t n, T *res, const Less &less){
	*res = *a;
	size_t i, j;
	for(i=0, j=2; j<n; i = j, j = (j<<1)+2){
		if(less(a[j],a[j-1]))j--;
		a[i] = a[j];
	}
	if(j==n)a[i] = a[j-1], i = j-1;
	return i;
}
struct Random {
    uint64_t rnd = 0;

    int operator()() {
        rnd = 1664525L * rnd + 1013904223L;
        return (int)(rnd & 0x7FFFFFFF);
    }
};

//========================= TEST ============
uint64_t ncmp=0;

extern "C" int icmp(const void *a,const void *b){
        ncmp++;
        int lhs_value = *(const int *)a;
        int rhs_value = *(const int *)b;
        return lhs_value < rhs_value ? -1 : lhs_value > rhs_value ? 1 : 0;
}

template<class T, class Less>
void sort_simple(T *a, size_t b, const Less &less){
	while(b > 1u){
		size_t k=0;
		for(size_t i=1;i<b;i++)
			if(less(a[k],a[i]))
				k=i;
		std::swap(a[0],a[k]);
		a++; b--;
	}
}
template<class T>
void aswap(T *arr1, T *arr2, size_t l){ 
	while(l--) std::swap(*(arr1++), *(arr2++));
}
// arr1(-l1..0] :merge: arr2(-l2..0] -> arr2(-l2..l1]
template<class T, class Less>
size_t backmerge(T *arr1, size_t l1, T *arr2, size_t l2, const Less &less){
	T *arr0=arr2+l1;
	for(;;){
		if(less(*arr1,*arr2)){ 
			std::swap(*(arr1--), *(arr0--));
			if(--l1==0) return 0;
		}else{ 
			std::swap(*(arr2--), *(arr0--));
			if(--l2==0) break;
		}
	}
	size_t res=l1; 
	do {
        std::swap(*(arr1--),*(arr0--));
    } while(--l1);
	return res;
}
// merge arr[p0..p1) by buffer arr[p1..p1+r)
template<class T, class Less>
void rmerge(T *arr, size_t l, size_t r, const Less &less){
	for(size_t i=0;i<l;i+=r){
		// select smallest arr[p0+n*r]
		size_t q=i;
		for(size_t j=i+r;j<l;j+=r){
			if(less(arr[q],arr[j])) q=j;
		}		
		if(q!=i) aswap(arr+i,arr+q,r); // swap it with current position
		if(i!=0){
			aswap(arr+l,arr+i,r);  // swap current position with buffer
			backmerge(arr+(l+r-1),r,arr+(i-1),r,less); // buffer :merge: arr[i-r..i) -> arr[i-r..i+r)
		}
	}
}
int rbnd(size_t len){
	len=len/2;
	size_t k=0;
	for(size_t i=1;i<len;i*=2) k++;
	len/=k;
	for(k=1;k<=len;k*=2) ;
	return (int)k;
}

template<class T, class Less>
void msort(T *arr, size_t len, const Less &less){  // ??? What is the type of sort algorithm? Stable sort???
	if(len<=15) {
        sort_simple(arr, len, less);
        return;
    }
	size_t r  = rbnd(len);
	size_t lr = (len/r-1)*r;
	for(size_t p=2; p<=lr; p+=2){
		if(less(arr[p-2], arr[p-1]))
			std::swap(arr[p-2], arr[p-1]);
		if(p&2) continue;
		aswap(arr+(p-2), arr+p, 2);
		size_t m=len-p;
		size_t q=2;
		for(;;){
			size_t q0=2*q;
			if(q0>m || (p&q0)) break;
			backmerge(arr+(p-q-1),q,arr+(p+q-1),q,less);
			q=q0;
		}
		backmerge(arr+(p+q-1), q, arr+(p-q-1),q,less);
		size_t q1=q;
		q*=2;
		while((q&p)==0){
			q*=2;
			rmerge(arr+(p-q),q,q1,less);
		}
	}
	size_t q1=0;
	for(size_t q=r;q<lr;q*=2) if((lr&q)!=0){
		q1+=q;
		if(q1!=q) rmerge(arr+(lr-q1),q1,r,less);
	}
	size_t s=len-lr;
	msort(arr+lr,s,less);
	aswap(arr,arr+lr,s);
	s+=backmerge(arr+(s-1),s,arr+(lr-1),lr-s,less);
	msort(arr,s,less);
}

} // namespace _sort_help

//---------------------------------------------------------------------------
//T may be a struct or a class ; must be defined correct  T::operator=(T&);
//KeyType must be an integer-type
template <typename T, typename Less>
void heap_sort(T *p, size_t n, const Less& less){
	size_t i;
	if(n<=1 || !p)return;
	for(i=n/2+1; i--;)			//First step
		_sort_help::UpdateHeap(p, i, n, p[i], less);

	//Now root is the maximum element of Arr[]
	for(i=n-1; i>0; i--){
		T temp = p[i];
		p[i] = p[0];
		_sort_help::UpdateHeap(p, 0, i, temp, less);
	}
}

//---------------------------------------------------------------------------
template<class T, class Less>
void merge_sort(T *a, T *temp, size_t n, const Less &less){
	const int st0 = 16;
	if(n <= 1)return;
	if(n <= st0)return _sort_help::bubbleSort(a,n,less);
	size_t sz;
	for(sz = 1; sz<n; )sz <<= 1;
	sz>>=2;
	merge_sort(a, temp, sz, less);
	merge_sort(a+sz, temp, sz, less);
	size_t sz1 = sz;
	while(n<2*sz+sz1)
        sz1 >>= 1;
    merge_sort(a+2*sz, temp, sz1, less);
    merge_sort(a+2*sz+sz1, temp, n-2*sz-sz1, less);
	_sort_help::merge(a, a+sz, sz, sz, temp, less);
	_sort_help::merge(a+2*sz, a+2*sz+sz1, sz1, n-2*sz-sz1, temp+2*sz, less);
	_sort_help::merge(temp, temp+2*sz, 2*sz, n-2*sz, a, less);
}
//---------------------------------------------------------------------------
template<class T, class Less>
size_t chooseMedian3(T*a, size_t n, const Less &less, _sort_help::Random &random){
	size_t st1=random()%n;
	size_t st2=random()%n;
	size_t st3=random()%n;
	if(less(a[st1], a[st2])){
		if(less(a[st2], a[st3])) return st2;
		else if(less(a[st1], a[st3])) return st3;
		else return st1;
	}else{
		if(less(a[st1], a[st3])) return st1;
		else if(less(a[st2], a[st3])) return st3;
		else return st2;
	}
}
template<int n0, class T, class Less>
size_t chooseMedian(T*a, size_t n, const Less &less, _sort_help::Random &random){
	size_t st[n0];
	for(int i=0; i<n0; i++)st[i] = random()%n;
	for(int i=1; i<n0; i++){
		for(int j=i; j--;) {
            if (less(a[st[j]], a[st[j + 1]]))
                break;
            else
                std::swap(st[j], st[j + 1]);
        }
	}
	return st[n0/2];
}
template<int n0, class T, class Less>
inline size_t chooseMedianDet(T*a, size_t n, const Less &less){
    size_t st[n0];
    int step = 1;//n/(n0+1);
    for(int i=0; i<n0; i++)
        st[i] = n/2+step*(i+1);
    for(int i=1; i<n0; i++){
        for(int j=i; j--;) {
            if (less(a[st[j]], a[st[j + 1]]))
                break;
            else
                std::swap(st[j], st[j + 1]);
        }
    }
    return st[n0/2];
}

//---------------------------------------------------------------------------
template<class T, class Less>
void quick_sort(T *a, size_t n, const Less &less) {//, _sort_help::Random &random){
	size_t n0 = n;
	if(n<=1)return;
	for(int j=0; n>1; j++){
		if((size_t(1)<<(j>>2))>n0)return heap_sort(a, n, less);
		//if(n<=0x3FFF/sizeof(T))return heap_sort(a, n, less);
		if(n<=15){ _sort_help::bubbleSort(a, n, less); break;}
        size_t st;
		if(n>=128){
			//if(n<1000)
				//st = chooseMedian5<5,T,Less>(a,n,less,random);
                st = chooseMedianDet<5,T,Less>(a,n,less);
            //else
                //st = chooseMedianDet<8,T,Less>(a,n,less);
			//else st = chooseMedian5<7,T,Less>(a,n,less);
		} else st=n/2;//random() % n;//abs((rand()<<16)+rand())%n;
		std::swap(a[st], a[n-1]);
		auto i = _sort_help::qSortSplit(a, n, less);
		if(i <= (n>>1)){
			quick_sort(a, i, less);
			a += i+1; n -= i+1;
		}else{
			quick_sort(a+i+1, n-i-1, less);
			n = i;
		}
	}
}



template<class T, class Less>
bool check(T *a, int n, const Less &less){
	for(int i=1; i<n; i++)if(!less(a[i-1],a[i])){printf("error at %d\n",i); return false;}
	return true;
}

