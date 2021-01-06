/*
Abstract Producer-Consumer Queue

Two implementations: fixed-sized array backed (nonzero elm_sz), or linked-list backed (zero elm_sz)
*/

#ifndef PC_QUEUE_H
#define PC_QUEUE_H
#include "common.h"

struct pc_queue;

int pc_queue_init(struct pc_queue* q, size_t sz, size_t elm_sz);
void pc_queue_deinit(struct pc_queue* q);
void pc_queue_enqueue(struct pc_queue* q, void* elm);
void pc_queue_dequeue(struct pc_queue* q, void* elm);

#endif