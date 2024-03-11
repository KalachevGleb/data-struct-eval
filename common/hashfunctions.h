#pragma once
#include <string>
using namespace std;

// TODO: following hash functions are not fast; need to replace them with fast hash functions
template <typename Int>
size_t hf_int(Int key, size_t size)
{
	static long long p=(long long)9519258779;
	static long long a=(rand()+((long long)(rand())<<16)+((long long)(rand())<<32))%p;
	static long long b=(rand()+((long long)(rand())<<16)+((long long)(rand())<<32))%p;
	return (size_t)(((key*a+b)%p)%size);
}

template <typename Int>
size_t hf_ka_int(Int key, size_t size)
{
	static long long p=(long long)9519258779;
	static long long a=(rand()+((long long)(rand())<<16)+((long long)(rand())<<32))%p;
	static long long b=(rand()+((long long)(rand())<<16)+((long long)(rand())<<32))%p;
	return (size_t)(((key*a+b)%p)%size);
}
size_t hf_string(const char* key, size_t size);
size_t hf_string(const string& key, size_t size);
