#include "comparator.h"
#include "common.h"
#include "error.h"

int int_comp(void* a, void* b){
	return (int)(*(uint32_t*)a - *(uint32_t*)b);
}
int nint_comp(void* a, void* b){
	return (int)*((uint32_t*)b - *(uint32_t*)a);
}
int uint_comp(void* a, void* b){
	return (int)(*(uint32_t*)a - *(uint32_t*)b);
}
int nuint_comp(void* a, void* b){
	return (int)(*(uint32_t*)b - *(uint32_t*)a);
}
int long_comp(void* a, void* b){
	return (int)(*(int64_t*)a - *(int64_t*)b);
}
int nlong_comp(void* a, void* b){
	return (int)(*(int64_t*)b - *(int64_t*)a);
}
int ulong_comp(void* a, void* b){
	return (int)(*(uint64_t*)a - *(uint64_t*)b);
}
int nulong_comp(void* a, void* b){
	return (int)(*(uint64_t*)b - *(uint64_t*)a);
}
int float_comp(void* a, void* b){
	return (int)(*(float*)a - *(float*)b);
}
int nfloat_comp(void* a, void* b){
	return (int)(*(float*)b - *(float*)a);
}
int double_comp(void* a, void* b){
	return (int)(*(double*)a - *(double*)b);
}
int ndouble_comp(void* a, void* b){
	return (int)(*(double*)b - *(double*)a);
}
int char_comp(void* a, void* b){
	return (int)(*(char*)a - *(char*)b);
}
int nchar_comp(void* a, void* b){
	return (int)(*(char*)b - *(char*)a);
}
int ptr_comp(void* a, void* b){
	return (int)((char*)a - (char*)b);
}
int nptr_comp(void* a, void* b){
	return (int)((char*)b - (char*)a);
}
int nstrcmp(void* a, void* b){
	return -strcmp(a, b);
}

static int (*comparator_array[])(void*, void*) = {
	[COMPARATOR_INT] int_comp,
	[COMPARATOR_INT] nint_comp,
	[COMPARATOR_UINT] uint_comp,
	[COMPARATOR_UINT] nuint_comp,
	[COMPARATOR_LONG] long_comp,
	[COMPARATOR_LONG] nlong_comp,
	[COMPARATOR_ULONG] ulong_comp,
	[COMPARATOR_ULONG] nulong_comp,
	[COMPARATOR_FLOAT] float_comp,
	[COMPARATOR_FLOAT] nfloat_comp,
	[COMPARATOR_DOUBLE] double_comp,
	[COMPARATOR_DOUBLE] ndouble_comp,
	[COMPARATOR_CHAR] char_comp,
	[COMPARATOR_CHAR] nchar_comp,
	[COMPARATOR_PTR] ptr_comp,
	[COMPARATOR_PTR] nptr_comp,
	[COMPARATOR_STR] strcmp,
	[COMPARATOR_STR] nstrcmp
};

struct comparator{
	int (*func)(void*, void*);
	off_t off;
};

int comparator_init(struct comparator* c, int func, off_t off){
	if (func < COMPARATOR_MIN || func > COMPARATOR_MAX){
		fail_out(EINVAL);
	}
	c->func = comparator_array[func];
	c->off = off;
	return 0;
}

int comparator_comp(struct comparator* c, void* a, void* b){
	return c->func((char*)a + c->off, (char*)b + c->off);
}
