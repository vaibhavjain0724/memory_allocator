#include "arena.h"
#include <sys/mman.h>

arena_t *arena_head = NULL;
static arena_t *arena_mover = NULL;


bool create_arena() {
  size_t total_size = ARENA_SIZE + sizeof(arena_t);
  int permissions = PROT_READ | PROT_WRITE;
  int flags = MAP_PRIVATE | MAP_ANON;
  int file = -1;
  int offset = 0;
  void *memory = mmap(NULL,       // address,
                      total_size, // size,
                      permissions,
                      flags, // type of mapping
                      file, offset);
  if (memory == MAP_FAILED) {
    return false;
  }
  arena_t *new_arena = memory;
  new_arena->memory = (char *)memory + sizeof(arena_t);
  new_arena->size = ARENA_SIZE;
  new_arena->used = 0;
  new_arena->block_head = NULL;
  new_arena->block_tail = NULL;
  new_arena->next = NULL;
  if (arena_head == NULL) {
    arena_head = new_arena;
    arena_mover = new_arena;
  } else {
    arena_mover->next = new_arena;
    arena_mover = arena_mover->next;
  }
  return true;
}

block_t *create_block(size_t size, arena_t *arena) {
  if (size > ARENA_SIZE)
    return NULL;

  block_t *newBlock = (block_t *)((char *)arena->memory + arena->used);
  newBlock->size = size;
  if (arena->block_head == NULL) {
    arena->block_head = newBlock;
    arena->block_tail = newBlock;
  } else {
    arena->block_tail->next = newBlock;
    arena->block_tail = newBlock;
  }
  newBlock->free = false;
  newBlock->next = NULL;
  return newBlock;
}