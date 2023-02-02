#include "list.h"
#include "error.h"

#define A_LIST_INIT_SZ 4

int a_list_init(struct a_list* ls, size_t init_sz, size_t elm_sz){
	ls->elm_sz = elm_sz;
	ls->cap = 0;
	if (init_sz == 0)
		ls->sz = A_LIST_INIT_SZ;
	else
		ls->sz = pow2_roundup(init_sz);
	if ((ls->list = malloc(ls->sz * elm_sz)) == NULL){
		failout(ENOMEM);
	}
	return 0;
}

void a_list_deinit(struct a_list* ls){
	void* it;
	freec(ls->list);
}

void* a_list_add(struct a_list* list){
	if (!(list->sz & (list->sz - 1))){
		realloc(list->list, (list->sz << 1) * list->elm_sz);
		if (list->list == NULL){
			failout(ENOMEM);
		}
	}
	list->sz++;
	return (void*)(a_list_index(list, list->sz - 1));
}

int a_list_del(struct a_list* list, idx_t i){
	if (list->sz <= 0){
		failout(E_EMPTY);
	}
	memcpy(a_list_index(list, i), a_list_index(list, list->sz - 1), list->elm_sz);
	list->sz--;
	return 0;
}

int a_list_del_last(struct a_list* list){
	if (list->sz <= 0){
		fail_out(E_EMPTY);
	}
	//memset(a_list_index(list, list->cap - 1), 0, list->elm_sz);
	list->sz--;
	return 0;
}

int a_list_trim(struct a_list* list){
	realloc(list->list, list->sz * list->elm_sz);
	return 0;
}
