#ifndef D_HASH_H
#define D_HASH_H

struct d_hash;
struct d_hash_bucket;

// d_hash_init: Initialize dynamic hash with a given bucket size
int d_hash_init(struct d_hash* dh, size_t bucket_sz, uint32_t (*hash_func)(void*));

// d_hash_deinit: Deinitialize dynamic hash
void d_hash_deinit(struct d_hash* dh);

int d_hash_insert(struct d_hash* dh, void* elm);

int d_hash_delete(struct d_hash* dh, void* elm);

/*int d_hash_search(struct d_hash* dh, void* elm);*/

#endif