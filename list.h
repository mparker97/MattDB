#ifndef LIST_H
#define LIST_H
#include <stdlib.h>
#include "common.h"
#include "mem_alloc.h"

// Materialize these as heads

struct l_list{
	struct l_list* next;
};

struct l_list_t{
	struct l_list* next;
	struct l_list* tail;
};

#define l_list_init(x) (x)->next = NULL
#define l_list_init_to(x, v) (x)->next = (v)
#define l_list_t_init(x) do{ l_list_init(x); (x)->tail = NULL;} while (0)
#define l_list_t_init_to(x, v) do{ l_list_init(x, v); (x)->tail = NULL;} while (0)

// insert 'n' after 'curr'
static inline void l_list_add_after(struct l_list* curr, struct l_list* n){
	n->next = curr->next;
	curr->next = n;
}

// insert and return a newly allocated heap node after 'curr'
static inline struct l_list* l_list_madd_after(struct l_list* curr){
	struct l_list* ret = malloc_fulfill(sizeof(struct l_list));
	if (ret != NULL){
		l_list_add_after(curr, ret);
	}
	return ret;
}

// insert 'n' after 'curr' and update the tail pointer if 'n' is last
static inline void l_list_t_add_after(struct l_list_t* head, struct l_list* curr, struct l_list* n){
	l_list_add_after(curr, n);
	if (n->next == NULL)
		head->tail = n;
}

// insert and return a newly allocated heap node after 'curr' and update the tail pointer if it is last
static inline struct l_list* l_list_t_madd_after(struct l_list_t* head, struct l_list* curr){
	struct l_list* ret = malloc_fulfill(sizeof(struct l_list));
	if (ret != NULL){
		l_list_t_add_after(head, curr, ret);
	}
	return ret;
}

// unlink and return the node after 'curr'
static inline struct l_list* l_list_del_after(struct l_list* curr){
	struct l_list* ret = curr->next;
	if (ret)
		curr->next = ret->next;
	return ret;
}

// unlink and return the node after 'curr' and update the tail pointer if it is last
static inline struct l_list* l_list_t_del_after(struct l_list_t* head, struct l_list* curr){
	struct l_list* ret = l_list_del_after(curr);
	if (curr->next == NULL && curr != (struct l_list*)head)
		head->tail = curr;
	return ret;
}

// insert 'n' at the end and update the tail pointer to it
static inline void l_list_t_add_tail(struct l_list_t* head, struct l_list* n){
	l_list_add_after(head->tail, n);
	head->tail = n;
}

// insert and return a newly allocated heap node at the end and update the tail pointer to it
static inline struct l_list* l_list_t_madd_tail(struct l_list_t* head, struct l_list* n){
	struct l_list* ret = l_list_madd_after(head->tail, n);
	head->tail = n;
	return ret;
}

#define l_list_foreach(ls, typ, elm, it) \
	for (it = ((ls)->next)? container_of(typ, elm, (ls)->next) : NULL; \
		it != NULL; \
		it = ((it)->elm.next)? container_of(typ, elm, (it)->elm.next) : NULL)

// Array-backed amortized list

struct a_list{
	void* list; // ptr to the list
	size_t sz; // absolute number of elements
	size_t elm_sz; // size of each element
};

int a_list_init(struct a_list* ls, size_t init_sz, size_t elm_sz);
void a_list_deinit(struct a_list* ls);
void* a_list_add(struct a_list* list);
int a_list_del(struct a_list* list, idx_t i);
int a_list_del_last(struct a_list* list);
//int a_list_trim(struct a_list* list);

#define a_list_index(ls, i) ((void*)((char*)((ls)->list) + (ls)->elm_sz * (i)))
#define a_list_foreachi(ls, it, i) for ((it) = a_list_index(ls, (i) = 0), (i) < (ls)->sz, it = a_list_index(ls, ++(i))

#define A_LIST_UNION(t, n0, n1, n2, n3) \
	union{ \
		struct{ \
			t* n0; \
			size_t n1; \
			size_t n2; \
		}; \
		struct a_list n3; \
	}

//#define a_list_sort(ls) (qsort((ls)->list, (ls)->sz, (ls)->elm_sz))

#endif