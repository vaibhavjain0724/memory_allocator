#include <stdio.h>
#include <stdbool.h>
#include <sys/mman.h>


typedef struct block {
    size_t size;
    bool free;
    struct block *next;

    
} block_t;


//arenas
#define ARENA_SIZE ( 1024 * 1024) 
typedef struct arena {
    void *memory;
    size_t size;

    block_t *block_head;
    block_t *block_tail;

    size_t used;
    
    struct arena *next;
} arena_t;

static arena_t *arena_head = NULL;
static arena_t *arena_mover = NULL;
bool create_arena(){
    size_t total_size = ARENA_SIZE + sizeof(arena_t);
    int permissions = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE | MAP_ANON;
    int file = -1;
    int offset = 0;
    void *memory = mmap(
        NULL, //address,
        total_size, //size,
        permissions,
        flags,//type of mapping
        file,
        offset
    );
    if(memory == MAP_FAILED){
        return false;
    }
    arena_t* new_arena = memory;
    new_arena -> memory = (char*)memory + sizeof(arena_t);
    new_arena -> size = ARENA_SIZE;
    new_arena -> used = 0;
    new_arena -> block_head = NULL;
    new_arena -> block_tail = NULL;
    new_arena -> next = NULL;
    if(arena_head == NULL){
        arena_head = new_arena;
        arena_mover = new_arena;
    }
    else{
        arena_mover -> next = new_arena;
        arena_mover = arena_mover -> next;
    }
    return true;
}



block_t *create_block(size_t size, arena_t* arena){
    if(size > ARENA_SIZE )return NULL;

    block_t *newBlock =(block_t*)( (char *)arena -> memory + arena -> used);
    newBlock -> size = size;
    if(arena -> block_head == NULL){
        arena -> block_head = newBlock;
        arena -> block_tail = newBlock;
    }
    else{
        arena -> block_tail -> next = newBlock;
        arena -> block_tail = newBlock;
    }
    newBlock -> free = false;
    newBlock -> next = NULL;
    return newBlock;

}

size_t align_size( size_t size ){
   if(size % 8 == 0 )return size;

   return (((size/8) + 1 )* 8);
}



void *my_malloc( size_t size ){
    size = align_size(size);


    if(ARENA_SIZE < size + sizeof(block_t)) return NULL;

    arena_t *arena_traverser = arena_head;
    size_t true_size = size + sizeof(block_t);
    while(arena_traverser != NULL){
        block_t* traverser = arena_traverser -> block_head;
        while(traverser != NULL){
        if(traverser -> size >= size && traverser -> free == true){
            traverser -> free = false;
            return (char*)traverser + sizeof(block_t);
        }
        traverser = traverser -> next;
        }
        if(arena_traverser -> size - arena_traverser -> used >= true_size){
            block_t* memory = create_block(size, arena_traverser);
            arena_traverser -> used += true_size; 
            return (char*)memory + sizeof(block_t);
        }
        arena_traverser = arena_traverser -> next;
    }
    bool success = create_arena();
    if(!success){
        return NULL;
    }
    return my_malloc(size);
    

   
   
} 

void my_free(void *ptr){
    if(ptr == NULL) return;

    block_t* block = (block_t*)((char*)ptr - sizeof(block_t));
    block -> free = true;
}

void check_block(void *ptr) {
    block_t *block = (block_t*)((char*)ptr - sizeof(block_t));

    printf("size = %zu\n", block->size);
    printf("free = %d\n", block->free);
}