//mmap
//asks os for a region of virtual memory

#include <stdio.h>


void *my_malloc( size_t size);

int main(){
    int *p = my_malloc(sizeof(int));
    
    *p = 42;

    printf("%d\n", *p);
    return 0;
}