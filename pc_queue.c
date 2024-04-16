#include "pc_queue.h"
#include <pthread.h>
#include "list.h"
#include "error.h"

#define LL_PC_QUEUE(x) ((x)->elm_sz == 0) // unused

struct pc_queue{
	union{
		struct{
			void* queue; // pointer to queue
			size_t queue_sz; // max size
		};
		l_list_t list; // linked list
	};
	size_t cap; // current capacity
	size_t elm_sz; // element size, 0 implies using linked list
	pthread_mutex_t lock;
	pthread_cond_t empty;
	pthread_cond_t full;
};

int pc_queue_init(struct pc_queue* q, size_t sz, size_t elm_sz){
	if (elm_sz == 0){ // linked-list backed
		l_list_t_init(&q->list);
	}
	else{ // fixed-sized array back
		if ((q->queue = malloc(sz * elm_sz)) == NULL){
			fail_out(ENOMEM);
		}
		q->queue_sz = sz;
	}
	q->cap = 0;
	q->elm_sz = elm_sz;
	pthread_mutex_init(&q->lock, NULL);
	pthread_cond_init(&q->empty);
	pthread_cond_init(&q->full);
	return 0;
}

void pq_queue_deinit(struct pc_queue* q){
	freec(q->queue);
}

void pc_queue_enqueue(struct pc_queue* q, void* elm){
	pthread_mutex_lock(&q->lock);
	while (q->cap == q->sz){
		pthread_cond_wait(&q->full, &q->lock);
	}
	memcpy((char*)(q->queue) + q->cap * q->elm_sz, elm, q->elm_sz);
	q->cap++;
	pthread_cond_signal(&q->empty);
	pthread_mutex_unlock(&q->lock);
}

void pc_queue_enqueue_LL(struct pc_queue* q, void* elm){
	pthread_mutex_lock(&q->lock);
	l_list_t_add_tail(&q->list, elm);
	q->cap++;
	pthread_cond_signal(&q->empty);
	pthread_mutex_unlock(&q->lock);
}

void pc_queue_dequeue(struct pc_queue* q, void* elm){
	pthread_mutex_lock(&q->lock);
	while (q->cap == 0){
		pthread_cond_wait(&q->empty, &q->lock);
	}
	q->cap--;
	memcpy(elm, (char*)(q->queue) + q->cap * q->elm_sz, q->elm_sz);
	pthread_cond_signal(&q->full);
	pthread_mutex_unlock(&q->lock);
}

void* pc_queue_dequeue_LL(struct pc_queue* q){
	void* ret;
	pthread_mutex_lock(&q->lock);
	while (q->cap == 0){
		pthread_cond_wait(&q->empty, &q->lock);
	}
	q->cap--;
	ret = l_list_t_del_after(&q->list, (struct l_list*)(&q->list));
	pthread_cond_signal(&q->full);
	pthread_mutex_unlock(&q->lock);
	return ret;
}
