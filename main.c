#include <stdio.h>
#include <stdbool.h>

void *my_malloc(size_t size);
void my_free(void *ptr);
void check_block(void *ptr);

int main() {
    int *p = my_malloc(sizeof(int));

    *p = 42;

    check_block(p);

    my_free(p);

    check_block(p);

    return 0;
}