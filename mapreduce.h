#ifndef MAP_REDUCE_H
#define MAP_REDUCE_H
#include "common.h"
#include "error.h"

struct map_reduce;

/*
map_reduce_init: Initialize the map reduce instance to operate on Table T with a comparator specified by func, an offset of the key in the record key_off, an offset of the value in the record val_off, num_mappers mappers, and num_reducer reducers.
*/
int map_reduce_init(struct map_reduce* mr, Table* T, int func, off_t key_off, off_t val_off, int num_mappers, int num_reducers);

/*
map_reduce_deinit: Deinitialize the map reduce instance.
*/
void map_reduce_deinit(struct map_reduce* mr);

/*
run_mr: Run the map reduce instance.
*/
int run_mr(struct map_reduce* mr);

#endif