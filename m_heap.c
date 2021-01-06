#include "m_heap.h"
#include "common.h"
#include "comparator.h"
#include "list.h"
#include "error.h"

struct m_heap{
	struct a_list heap;
	struct comparator* c;
};

int m_heap_init(struct m_heap* mh, size_t elm_sz, struct comparator* c){
	mh->c = c;
	return a_list_init(&mh->heap, 0, elm_sz);
}

void m_heap_deinit(struct m_heap* mh){
	a_list_deinit(mh->heap);
}

int m_heap_insert(struct m_heap* mh, void* elm){ // with dups
	char* n;
	off_t curr, next;
	n = a_list_add(&mh->heap);
	if (!PERR(n)){
		for (curr = n - &mh->heap.list, n = &mh->heap.list, next = curr / 2; next >= mh->heap.elm_sz && comparator_comp(mh->c, n + next, elm) > 0; curr = next, next /= 2){
			memcpy(n + curr, n + next, mh->heap.elm_sz);
		}
		memcpy(n + curr, elm, mh->heap.elm_sz);
		return 0;
	}
	return PTR_ERR(n);
}

int m_heap_delete(struct m_heap* mh, void* elm){
	char* n;
	off_t curr, next, last;
	if (mh->heap.cap <= 0){
		return -E_EMPTY;
	}
	n = &mh->heap.list;
	memcpy(elm, n, mh->heap.elm_sz);
	last = (mh->heap.cap - 1) * mh->heap.elm_sz;
	for (curr = 0, next = curr * 2 + mh->heap.elm_sz; next < last; curr = next, next = curr * 2 + mh->heap.elm_sz){
		if (next + mh->heap.elm_sz <= last && comparator_comp(mh->c, n + next, n + next + mh->heap.elm_sz) < 0){
			next += mh->heap.elm_sz;
		}
		if (comparator_comp(mh->c, n + next, n + last) < 0){
			next = last;
		}
		memcpy(n + curr, n + next, mh->heap.elm_sz);
	}
	return a_list_del_last(&mh->heap);
}
