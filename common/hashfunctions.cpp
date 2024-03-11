#include "hashfunctions.h"

size_t hf_string(const char* key, size_t size)
{
	size_t res=0,n;
	for(n=0;*key!='\0';key++,n++)
		res=(res+(*key)*n)%size;
	return res%size;
}

size_t hf_string(const string &Key, size_t size)
{
	size_t res=0, n, sz=Key.length();
	const char *key=Key.c_str();
	for(n=0; n<sz; key++, n++)
		res=(res+(*key)*n)%size;
	return res%size;
}

