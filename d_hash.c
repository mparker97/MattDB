#include "d_hash.h"
#include "list.h"
#include "common.h"

struct d_hash{
	struct a_list al;
	uint32_t (*hash_func)(void*);
	size_t bucket_sz;
};

struct d_hash_bucket{
	size_t cap;
	uint32_t level;
	char bucket[];
};

int d_hash_init(struct d_hash* dh, size_t bucket_sz, uint32_t (*hash_func)(void*)){
	int ret;
	if ((ret = a_list_init(&dh->al, 0, sizeof(struct d_hash_bucket*))) < 0){
		return ret;
	}
	dh->hash_func = hash_func;
	dh->bucket_sz = bucket_sz;
	return 0;
}

void d_hash_deinit(struct d_hash* dh){
	a_list_deinit(&dh->al);
}

int d_hash_insert(struct d_hash* dh, void* elm){
	struct d_hash_bucket* bucket;
	uint32_t hash;
	off_t i, free_space;
	
	hash = dh->hash_func(elm);
	
	bucket = (struct d_hash_bucket*)(a_list_index(dh->al, hash & (dh->al.sz - 1)));
	if (bucket->cap == dh->bucket_sz){
		// TODO: split bucket
	}
	
	for (i = 0; i < dh->bucket_sz * dh->al.elm_sz; i += dh->al.elm_sz){
		if ((void*)(bucket->bucket[i]) == NULL){
			free_space = i;
			break;
		}
		if ((void*)(bucket->bucket[i]) == elm){
			fail_out(EEXIST);
		}
	}
	for (i = 0; i < dh->bucket_sz * dh->al.elm_sz; i += dh->al.elm_sz){
		if ((void*)(bucket->bucket[i]) == elm){
			fail_out(EEXIST);
		}
	}
	
	// free space should exist
	(void*)(bucket->bucket[free_space]) = elm;
	return 0;
}

int d_hash_delete(struct d_hash* dh, void* elm){
	
}
