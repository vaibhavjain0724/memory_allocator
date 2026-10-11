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
  new_arena -> live_blocks = 0;
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
bool destroy_arena(arena_t *arena) {
  if (arena == NULL || arena->live_blocks != 0)
    return false;

  // Keep at least one arena mapped.
  if (arena == arena_head && arena->next == NULL)
    return false;

  arena_t *previous = NULL;
  arena_t *current = arena_head;

  while (current != NULL && current != arena) {
    previous = current;
    current = current->next;
  }

  if (current == NULL)
    return false;

  arena_t *next = arena->next;
  size_t total_size = sizeof(arena_t) + arena->size;

  // Unlink the arena before attempting to unmap it.
  if (previous == NULL)
    arena_head = next;
  else
    previous->next = next;

  if (arena_mover == arena)
    arena_mover = previous;

  if (munmap(arena, total_size) == 0)
    return true;

  // If munmap fails, restore the arena to the list.
  arena->next = next;

  if (previous == NULL)
    arena_head = arena;
  else
    previous->next = arena;

  if (arena_mover == previous || arena_mover == NULL)
    arena_mover = arena;

  return false;
}