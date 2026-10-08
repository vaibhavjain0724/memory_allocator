#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include "arena.h"
void *my_malloc(size_t size);
void my_free(void *ptr);
void check_block(void *ptr);
block_t *find_block(void * ptr);
void *my_calloc(size_t n, size_t size);
void *my_realloc(void *ptr, size_t size);
#endif