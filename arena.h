#ifndef ARENA_H
#define ARENA_H

#include <stdbool.h>
#include <stddef.h>

#define ARENA_SIZE (1024 * 1024)

typedef struct block {
  size_t size;
  bool free;
  struct block *next;
} block_t;

typedef struct arena {
  void *memory;
  size_t size;
  block_t *block_head;
  block_t *block_tail;
  size_t used;
  struct arena *next;
} arena_t;

extern arena_t *arena_head;

bool create_arena(void);
block_t *create_block(size_t size, arena_t *arena);

#endif