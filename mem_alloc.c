#include "mem_alloc.h"
#include "common.h"

static __thread size_t alloc_sz = 0;
static __thread off_t alloc_off = 0;

void malloc_plead(size_t sz, off_t off){
	alloc_sz = sz;
	alloc_off = off;
}

void* malloc_fulfill(size_t sz){
	void* ret = malloc(sz + alloc_sz);
	if (ret != NULL){
		ret = (void*)((char*)ret + alloc_off);
	}
	return ret;
}
