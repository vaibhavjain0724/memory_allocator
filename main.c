#include <stdio.h>
#include <stdbool.h>

void *my_malloc(size_t size);
void my_free(void *ptr);
void check_block(void *ptr);
#include <stdio.h>

void *my_malloc(size_t size);
void my_free(void *ptr);
void check_block(void *ptr);

int main() {
    int *a = my_malloc(sizeof(int));

    *a = 42;

    printf("a = %d\n", *a);

    check_block(a);

    my_free(a);

    check_block(a);

    int *b = my_malloc(sizeof(int));

    check_block(b);

    printf("a address = %p\n", (void *)a);
    printf("b address = %p\n", (void *)b);

    return 0;
}