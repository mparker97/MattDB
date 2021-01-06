#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

/*
malloc_plead: Set up a thread-local state so that future calls to malloc_fulfill(m) allocate a region of memory of size m + sz, returning a pointer at offset 'off' within this region.
*/
void malloc_plead(size_t sz, off_t off);

/*
malloc_fulfill: Fulfill a potential malloc offset and extra size request created by malloc_plead.
*/
void* malloc_fulfill(size_t sz);

#endif