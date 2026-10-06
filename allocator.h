#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

void *my_malloc(size_t size);
void my_free(void *ptr);
void check_block(void *ptr);

#endif