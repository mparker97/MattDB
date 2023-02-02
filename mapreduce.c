#include "map_reduce.h"
#include "common.h"
#include "error.h"
#include "pc_queue.h"
#include "m_heap.h"
#include "list.h"
#include "mem_alloc.h"

#include "Page.h"
#include "Record.h"
#include "Field.h"
#include <pthread.h>

#define value_list_list(v) ((struct l_list*)(v) - 1))
#define value_list_value(l) ((void*)((l) + 1))

struct map_reduce{
	Table* T;
	struct pq_queue* q;
	struct comparator key_comp;
	Field* F_key;
	Field* F_value;
	int num_mappers;
	int num_reducers;
};

static struct value_list_head{
	struct l_list list;
	void* key;
};

int map_reduce_init(struct map_reduce* mr, Table* T, Field* F_key, Field* F_value, int func, off_t key_off, off_t val_off, int num_mappers, int num_reducers){
	int ret = 0;
	if (key_off >= T->record_sz || val_off >= T->record_sz){ // TODO: record_sz
		fail_out(EINVAL);
	}
	comparator_init(&mr->key_comp, func, offset_of(struct key_list, key));
	mr->F_key = F_key;
	mr->F_value = F_value;
	mr->num_mappers = num_mappers;
	mr->num_reducers = num_reducers;
	ret = pc_queue_init(mr->q, num_mappers, sizeof(Page*));
fail:
	return ret;
}

void map_reduce_deinit(struct map_reduce* mr){
	mr->T = NULL;
	pc_queue_deinit(mr->q);
}

void* mr_mapper(void* arg){
	Page* P;
	Record* R;
	struct map_reduce* mr;
	struct value_list_head* n;
	struct l_list* ls;
	void* value;
	struct m_heap mh;
	int i, new = 0;
	
	mr = (struct map_reduce*)arg;
	ret = m_heap_init(&mh, sizeof(struct value_list_head), &mr->key_comp);
	if (ret < 0){
		fail_out(ret);
	}
	
	for (;;){
		pc_queue_dequeue(mr->q, &P);
		if (P == NULL){ // Done signal
			break;
		}
		// TODO: page_in / page_out?
		Page_foreach_Record(P, R, T->record_sz){
			// TODO
			n = m_heap_insert_unique(&mh, Field_value(mr->F_key, R), &new); // TODO: make function
			if (new){
				malloc_plead(0, 0);
				if ((n->key = Field_value_copy(mr->F_key, R)) == NULL){
					ret = n->key;
					// TODO error
				}
			}
			
			malloc_plead(sizeof(struct l_list), sizeof(struct list));
			value = Field_value_copy(mr->F_value, R);
			if (value == NULL){
				// TODO: error
			}
			l_list_add(&n->list, value_list_list(value));
		}
	}
	malloc_plead(0, 0);
	m_heap_deinit(&mh); // TODO: keep this around
	return NULL; // TODO
}

int run_mr(struct map_reduce* mr){
	pthread_t* mappers = NULL;
	Page* P = NULL;
	int ret = 0, i;
	
	if (mr->T == NULL || mr->q == NULL){
		fail_out(EUNINIT);
	}
	mappers = malloc(mr->num_mappers * sizeof(pthread_t));
	if (mappers == NULL){
		fail_out(ENOMEM);
	}
	
	for (i = 0; i < mr->num_mappers; i++){
		pthread_create(mappers + i, NULL, mr_mapper, mr);
	}
	table_foreach_page(mr->T, &P){
		pc_queue_enqueue(mr->q, &P);
	}
	P = NULL;
	for (i = 0; i < mr->num_mappers; i++){ // Send done signal to each
		pc_queue_enqueue(mr->q, &P);
	}
	
	free(mappers);
fail:
	return ret;
}
