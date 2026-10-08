#include "allocator.h"
#include "arena.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <pthread.h>

static pthread_mutex_t allocator_lock =
    PTHREAD_MUTEX_INITIALIZER;

size_t align_size(size_t size) {
  if (size % 8 == 0)
    return size;

  return (((size / 8) + 1) * 8);
}

static void *malloc_internal(size_t size) {
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

  return malloc_internal(size);
}

void *my_malloc(size_t size) {
  pthread_mutex_lock(&allocator_lock);

  void *ptr = malloc_internal(size);

  pthread_mutex_unlock(&allocator_lock);

  return ptr;
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


block_t *find_block(void *ptr) {
  arena_t *arena = arena_head;

  while (arena != NULL) {
    block_t *current = arena->block_head;

    while (current != NULL) {
      void *block_memory =
          (char *)current + sizeof(block_t);

      if (block_memory == ptr) {
        return current;
      }

      current = current->next;
    }

    arena = arena->next;
  }

  return NULL;
}

static void free_internal(void *ptr) {
  if (ptr == NULL)
    return;

  block_t *block =
      find_block(ptr);

  if (block == NULL)
    return;

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

void my_free(void *ptr) {
  pthread_mutex_lock(&allocator_lock);

  free_internal(ptr);

  pthread_mutex_unlock(&allocator_lock);
}

void *my_calloc(size_t n, size_t size) {

  if (size != 0 && n > SIZE_MAX / size)
    return NULL;

  size_t total_size = n * size;

  void *ptr = my_malloc(total_size);

  if (ptr == NULL)
    return NULL;

  memset(ptr, 0, total_size);

  return ptr;
}

void check_block(void *ptr) {
  block_t *block = find_block(ptr);

  if (block == NULL) {
    printf("Invalid block\n");
    return;
  }

  printf("size = %zu\n", block->size);
  printf("free = %d\n", block->free);
}

void *my_realloc(void *ptr, size_t size) {
  if (ptr == NULL) {
    return my_malloc(size);
  }

  if (size == 0) {
    my_free(ptr);
    return NULL;
  }

  block_t *block = find_block(ptr);

  if (block == NULL)
    return NULL;

  size_t copy_size = block->size;

  if (size < copy_size)
    copy_size = size;

  void *p = my_malloc(size);

  if (p == NULL)
    return NULL;

  memcpy(p, ptr, copy_size);

  my_free(ptr);

  return p;
}