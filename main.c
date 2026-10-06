#include <stdio.h>
#include <stddef.h>

void *my_malloc(size_t size);
void my_free(void *ptr);
void check_block(void *ptr);

int main() {

    void *a = my_malloc(100);
    void *b = my_malloc(100);
    void *c = my_malloc(100);

    printf("Addresses:\n");
    printf("a = %p\n", a);
    printf("b = %p\n", b);
    printf("c = %p\n", c);

    printf("\n--- Freeing b ---\n");
    my_free(b);

    printf("b: ");
    check_block(b);

    printf("\n--- Freeing a ---\n");
    my_free(a);

    printf("a: ");
    check_block(a);

    printf("\n--- Allocating 180 bytes ---\n");
    void *d = my_malloc(180);

    printf("d = %p\n", d);
    check_block(d);

    return 0;
}