#include "allocator.h"
#include "arena.h"
#include <stdio.h>

size_t align_size(size_t size) {
  if (size % 8 == 0)
    return size;

  return (((size / 8) + 1) * 8);
}

void *my_malloc(size_t size) {
  size = align_size(size);

  if (ARENA_SIZE < size + sizeof(block_t))
    return NULL;

  arena_t *arena_traverser = arena_head;
  size_t true_size = size + sizeof(block_t);

  while (arena_traverser != NULL) {
    block_t *traverser = arena_traverser->block_head;

    while (traverser != NULL) {
      if (traverser->size >= size && traverser->free == true) {

        if (traverser->size >= size + sizeof(block_t) + 8) {

          block_t *new_block =
              (block_t *)((char *)traverser + sizeof(block_t) + size);

          new_block->size =
              traverser->size - size - sizeof(block_t);

          new_block->free = true;

          new_block->next = traverser->next;
          traverser->next = new_block;

          if (traverser == arena_traverser->block_tail) {
            arena_traverser->block_tail = new_block;
          }

          traverser->size = size;
        }

        traverser->free = false;

        return (char *)traverser + sizeof(block_t);
      }

      traverser = traverser->next;
    }

    if (arena_traverser->size - arena_traverser->used >= true_size) {
      block_t *memory = create_block(size, arena_traverser);

      arena_traverser->used += true_size;

      return (char *)memory + sizeof(block_t);
    }

    arena_traverser = arena_traverser->next;
  }

  bool success = create_arena();

  if (!success) {
    return NULL;
  }

  return my_malloc(size);
}

void coalesce(arena_t *arena) {
  block_t *current = arena->block_head;

  while (current != NULL && current->next != NULL) {

    if (current->free && current->next->free) {

      block_t *next = current->next;

      current->size += sizeof(block_t) + next->size;
      current->next = next->next;

      if (next == arena->block_tail) {
        arena->block_tail = current;
      }

    } else {
      current = current->next;
    }
  }
}

void my_free(void *ptr) {
  if (ptr == NULL)
    return;

  block_t *block =
      (block_t *)((char *)ptr - sizeof(block_t));

  block->free = true;

  arena_t *arena = arena_head;

  while (arena != NULL) {
    block_t *current = arena->block_head;

    while (current != NULL) {

      if (current == block) {
        coalesce(arena);
        return;
      }

      current = current->next;
    }

    arena = arena->next;
  }
}

void check_block(void *ptr) {
  block_t *block =
      (block_t *)((char *)ptr - sizeof(block_t));

  printf("size = %zu\n", block->size);
  printf("free = %d\n", block->free);
}