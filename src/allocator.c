#include "allocator.h"
#include "arena.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>

static pthread_mutex_t allocator_lock = PTHREAD_MUTEX_INITIALIZER;

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

          new_block->size = traverser->size - size - sizeof(block_t);

          new_block->free = true;

          new_block->next = traverser->next;
          traverser->next = new_block;

          if (traverser == arena_traverser->block_tail) {
            arena_traverser->block_tail = new_block;
          }

          traverser->size = size;
        }

        traverser->free = false;

        arena_traverser->live_blocks++;

        return (char *)traverser + sizeof(block_t);
      }

      traverser = traverser->next;
    }

    if (arena_traverser->size - arena_traverser->used >= true_size) {
      block_t *memory = create_block(size, arena_traverser);

      if (memory == NULL)
        return NULL;

      arena_traverser->used += true_size;
      arena_traverser->live_blocks++;

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
      void *block_memory = (char *)current + sizeof(block_t);

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

  block_t *block = find_block(ptr);

  if (block == NULL || block->free)
    return;

  arena_t *arena = arena_head;

  while (arena != NULL) {
    block_t *current = arena->block_head;

    while (current != NULL) {
      if (current == block) {
        block->free = true;
        arena->live_blocks--;

        coalesce(arena);

        if (arena->live_blocks == 0) {
        destroy_arena(arena);
      }

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
  pthread_mutex_lock(&allocator_lock);

  if (size != 0 && n > SIZE_MAX / size) {
    pthread_mutex_unlock(&allocator_lock);
    return NULL;
  }

  size_t total_size = n * size;

  void *ptr = malloc_internal(total_size);

  if (ptr == NULL) {
    pthread_mutex_unlock(&allocator_lock);
    return NULL;
  }

  memset(ptr, 0, total_size);

  pthread_mutex_unlock(&allocator_lock);
  return ptr;
}

void check_block(void *ptr) {
  pthread_mutex_lock(&allocator_lock);

  block_t *block = find_block(ptr);

  if (block == NULL) {
    printf("Invalid block\n");
    pthread_mutex_unlock(&allocator_lock);
    return;
  }

  printf("size = %zu\n", block->size);
  printf("free = %d\n", block->free);

  pthread_mutex_unlock(&allocator_lock);
}
void *my_realloc(void *ptr, size_t size) {
  pthread_mutex_lock(&allocator_lock);

  if (ptr == NULL) {
    void *p = malloc_internal(size);
    pthread_mutex_unlock(&allocator_lock);
    return p;
  }

  if (size == 0) {
    free_internal(ptr);
    pthread_mutex_unlock(&allocator_lock);
    return NULL;
  }

  block_t *block = find_block(ptr);

  if (block == NULL) {
    pthread_mutex_unlock(&allocator_lock);
    return NULL;
  }

  size_t copy_size = block->size;

  if (size < copy_size)
    copy_size = size;

  void *p = malloc_internal(size);

  if (p == NULL) {
    pthread_mutex_unlock(&allocator_lock);
    return NULL;
  }

  memcpy(p, ptr, copy_size);
  free_internal(ptr);

  pthread_mutex_unlock(&allocator_lock);
  return p;
}