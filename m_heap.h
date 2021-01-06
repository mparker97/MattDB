// Abstract Minimum/Maximum Heap
#ifndef M_HEAP_H
#define M_HEAP_H
#include "comparator.h"

struct m_heap;

/*
m_heap_init: Initialize the m_heap with the given element size and comparator
*/
int m_heap_init(struct m_heap* mh, size_t elm_sz, struct comparator* c);

/*
m_heap_deinit: Deinitialize the m_heap.
*/
void m_heap_deinit(struct m_heap* mh);

/*
m_heap_insert: Insert an element into the m_heap.
*/
int m_heap_insert(struct m_heap* mh, void* elm);
//int m_heap_insert_unique(struct m_heap* mh, void* elm);

/*
m_heap_delete: Delete an element from the m_heap and copy its contents into elm.
*/
int m_heap_delete(struct m_heap* mh, void* elm);

#endif