#include <stdio.h>

void *my_malloc(size_t size);
void my_free(void *ptr);
void check_block(void *ptr);

int main() {
    int *a = my_malloc(sizeof(int));
    int *b = my_malloc(sizeof(int));
    int *c = my_malloc(sizeof(int));

    *a = 10;
    *b = 20;
    *c = 30;

    printf("a = %d, address = %p\n", *a, (void *)a);
    printf("b = %d, address = %p\n", *b, (void *)b);
    printf("c = %d, address = %p\n", *c, (void *)c);

    check_block(a);
    check_block(b);
    check_block(c);

    return 0;
}