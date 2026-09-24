#include <stdio.h>
#include <stdbool.h>
#include <sys/mman.h>

typedef struct block {
    size_t size;
    bool free;
    struct block *next;

    
} block_t;


static block_t *head = NULL;
static block_t *mover = NULL;

void *my_malloc( size_t size ){

    size_t total_size = sizeof(block_t) + size;
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
    if(memory == MAP_FAILED) return NULL;

    block_t *newBlock = memory;

    newBlock -> size = size;
    newBlock -> free = false;
    newBlock -> next = NULL;

    if(head == NULL){
        head = newBlock;
        mover = newBlock;
    }
    else{
        mover -> next = newBlock;
        mover = newBlock;
    }

    return (char *)memory + sizeof(block_t);
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