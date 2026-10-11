# Custom Memory Allocator in C

A custom memory allocator implemented in C that manages memory using `mmap()`, arenas, block metadata, and a linked-list-based allocation strategy. The project implements custom versions of `malloc`, `free`, `calloc`, and `realloc`, with block reuse, splitting, coalescing, and mutex-based synchronization.

## Features

- **Custom allocation API:** `my_malloc()`, `my_free()`, `my_calloc()`, and `my_realloc()`.
- **Arena-based memory management:** Requests memory from the operating system using `mmap()` and manages allocations within fixed-size arenas.
- **Block metadata:** Maintains allocation size, free status, and linked-list pointers for each block.
- **Memory reuse:** Reuses previously freed blocks instead of always requesting new memory.
- **Block splitting:** Splits sufficiently large free blocks to reduce wasted space.
- **Block coalescing:** Merges adjacent free blocks to reduce external fragmentation.
- **Multiple arenas:** Creates additional arenas when existing arenas cannot satisfy an allocation.
- **Memory release:** Uses `munmap()` to release eligible empty arenas back to the operating system.
- **Thread synchronization:** Uses a POSIX mutex to protect shared allocator metadata.
- **Zero-initialized allocation:** Implements `my_calloc()` with allocation-size overflow checking.
- **Memory resizing:** Implements `my_realloc()` while preserving existing data up to the smaller of the old and new sizes.

## Project Structure

```text
.
├── main.c
├── allocator.c
├── allocator.h
├── arena.c
└── arena.h
```

| File | Responsibility |
|---|---|
| `main.c` | Entry point and allocator tests |
| `allocator.c` | Allocation, deallocation, block reuse, splitting, coalescing, and synchronization |
| `allocator.h` | Public allocator API declarations |
| `arena.c` | Arena creation, management, and destruction |
| `arena.h` | Arena and block structures, constants, and declarations |

## Memory Layout

Each arena consists of metadata followed by a usable memory region.

```text
Arena mapping
┌─────────────────────────────┐
│         arena_t             │
│  Arena metadata             │
├─────────────────────────────┤
│         block_t             │
│  Size, free flag, next      │
├─────────────────────────────┤
│      User allocation        │
├─────────────────────────────┤
│         block_t             │
│  Size, free flag, next      │
├─────────────────────────────┤
│      User allocation        │
└─────────────────────────────┘
```

The default usable arena size is **1 MiB**. Arena metadata is stored inside the same mapping, so the total mapping size includes both `sizeof(arena_t)` and the usable arena memory.

Each block stores its size, whether it is free, and a pointer to the next block. The allocator uses this metadata to locate allocations, reuse freed blocks, and merge adjacent free blocks.

## How It Works

### 1. Allocation

`my_malloc()` acquires the allocator mutex and searches existing arenas for a sufficiently large free block. If no suitable block exists, it allocates from unused arena space or creates another arena using `mmap()`.

### 2. Deallocation

`my_free()` locates the block associated with the supplied pointer, marks it free, and coalesces adjacent free blocks. Invalid pointers and repeated frees are ignored by the current implementation.

### 3. Zero-initialized allocation

`my_calloc()` checks for multiplication overflow, allocates the requested memory, and initializes it to zero.

### 4. Reallocation

`my_realloc()` allocates a replacement block, copies the existing data that fits, and frees the original block. A `NULL` pointer is handled as an allocation request, while a zero-size request frees the existing allocation.

### 5. Arena cleanup

The allocator tracks the number of live allocations in each arena. When an arena becomes empty, it can be removed from the arena list and released with `munmap()`, provided at least one arena remains mapped.

### 6. Thread synchronization

A global `pthread_mutex_t` serializes public allocator operations that access shared metadata. Internal helper functions operate under the assumption that the caller already holds the mutex.

## Requirements

- A C compiler such as Clang or GCC
- macOS or another POSIX-compatible operating system supporting `mmap()`, `munmap()`, and pthreads

## Build and Run

Compile the project on macOS:

```bash
clang main.c allocator.c arena.c -o allocator -pthread
```

Run the executable:

```bash
./allocator
```

## Public API

```c
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_calloc(size_t n, size_t size);
void *my_realloc(void *ptr, size_t size);
void check_block(void *ptr);
```

Include `allocator.h` in files that use the allocator.

## Current Limitations

- Uses a single global mutex, so allocation operations are serialized.
- Uses a simple linked-list search strategy rather than a more advanced allocation index.
- `my_realloc()` currently allocates a new block rather than resizing the existing block in place.
- Alignment is handled with a fixed 8-byte alignment strategy; platform-specific alignment requirements need further consideration.
- Thread safety and arena reclamation should be validated with dedicated stress tests.
- The implementation is intended for learning and experimentation, not as a drop-in replacement for the system allocator.

## Learning Goals

This project explores low-level systems programming concepts, including:

- Dynamic memory management
- Virtual memory and memory mapping
- Pointer arithmetic and memory layout
- Linked lists and block metadata
- Memory fragmentation and coalescing
- Mutexes and concurrent access
- Resource cleanup and operating-system interfaces

## Future Improvements

- Multithreaded stress testing
- More comprehensive allocation and deallocation tests
- Improved alignment and overflow handling
- Better fragmentation management and allocation strategies
- In-place reallocation where possible
- More robust invalid-pointer diagnostics
- Performance benchmarking against the system allocator

