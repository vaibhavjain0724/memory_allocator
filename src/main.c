#include "allocator.h"
#include <stdio.h>
#include <string.h>

int main() {

    printf("========== TEST 1: BASIC MALLOC ==========\n");

    void *a = my_malloc(100);

    printf("a = %p\n", a);
    check_block(a);


    printf("\n========== TEST 2: ALIGNMENT ==========\n");

    void *b = my_malloc(9);

    printf("b = %p\n", b);
    check_block(b);


    printf("\n========== TEST 3: FREE + REUSE ==========\n");

    void *c = my_malloc(100);
    printf("c = %p\n", c);

    my_free(c);

    void *d = my_malloc(80);
    printf("d = %p\n", d);

    if (c == d)
        printf("PASS: freed block reused\n");
    else
        printf("FAIL: block not reused\n");


    printf("\n========== TEST 4: SPLITTING ==========\n");

    void *e = my_malloc(300);
    printf("e = %p\n", e);
    check_block(e);

    my_free(e);

    void *f = my_malloc(100);
    printf("f = %p\n", f);
    check_block(f);

    if (e == f)
        printf("PASS: block reused and split\n");
    else
        printf("FAIL: block not reused\n");


    printf("\n========== TEST 5: COALESCING ==========\n");

    void *g = my_malloc(100);
    void *h = my_malloc(100);
    void *i = my_malloc(100);

    printf("g = %p\n", g);
    printf("h = %p\n", h);
    printf("i = %p\n", i);

    my_free(h);
    my_free(g);

    void *j = my_malloc(180);

    printf("j = %p\n", j);
    check_block(j);

    if (j == g)
        printf("PASS: blocks coalesced and reused\n");
    else
        printf("FAIL: blocks did not coalesce\n");


    printf("\n========== TEST 6: CALLOC ==========\n");

    int *arr = my_calloc(5, sizeof(int));

    if (arr == NULL) {
        printf("FAIL: calloc returned NULL\n");
    } else {
        printf("calloc array: ");

        for (int x = 0; x < 5; x++) {
            printf("%d ", arr[x]);
        }

        printf("\n");

        int all_zero = 1;

        for (int x = 0; x < 5; x++) {
            if (arr[x] != 0) {
                all_zero = 0;
                break;
            }
        }

        if (all_zero)
            printf("PASS: calloc memory is zeroed\n");
        else
            printf("FAIL: calloc memory is not zeroed\n");
    }


    printf("\n========== TEST 7: REALLOC GROW ==========\n");

    int *numbers = my_malloc(3 * sizeof(int));

    numbers[0] = 10;
    numbers[1] = 20;
    numbers[2] = 30;

    printf("Before realloc:\n");
    printf("%d %d %d\n",
           numbers[0],
           numbers[1],
           numbers[2]);

    int *new_numbers =
        my_realloc(numbers, 6 * sizeof(int));

    if (new_numbers == NULL) {
        printf("FAIL: realloc returned NULL\n");
    } else {
        printf("After realloc:\n");
        printf("%d %d %d\n",
               new_numbers[0],
               new_numbers[1],
               new_numbers[2]);

        if (new_numbers[0] == 10 &&
            new_numbers[1] == 20 &&
            new_numbers[2] == 30) {
            printf("PASS: realloc preserved old data\n");
        } else {
            printf("FAIL: realloc corrupted old data\n");
        }
    }


    printf("\n========== TEST 8: REALLOC SHRINK ==========\n");

    int *values = my_malloc(5 * sizeof(int));

    values[0] = 100;
    values[1] = 200;
    values[2] = 300;
    values[3] = 400;
    values[4] = 500;

    int *smaller =
        my_realloc(values, 2 * sizeof(int));

    if (smaller == NULL) {
        printf("FAIL: realloc returned NULL\n");
    } else {
        printf("After shrinking:\n");
        printf("%d %d\n",
               smaller[0],
               smaller[1]);

        if (smaller[0] == 100 &&
            smaller[1] == 200) {
            printf("PASS: realloc preserved data while shrinking\n");
        } else {
            printf("FAIL: realloc corrupted data while shrinking\n");
        }
    }


    printf("\n========== TEST 9: REALLOC NULL ==========\n");

    void *r = my_realloc(NULL, 100);

    if (r != NULL)
        printf("PASS: realloc(NULL, size) works\n");
    else
        printf("FAIL: realloc(NULL, size) failed\n");


    printf("\n========== TEST 10: REALLOC ZERO ==========\n");

    void *s = my_malloc(100);

    void *result = my_realloc(s, 0);

    if (result == NULL)
        printf("PASS: realloc(ptr, 0) returned NULL\n");
    else
        printf("FAIL: realloc(ptr, 0) did not return NULL\n");


    printf("\n========== TEST 11: INVALID FREE ==========\n");

    int x = 42;

    my_free(&x);

    printf("PASS: allocator survived invalid free\n");


    printf("\n========== TEST 12: INVALID CHECK ==========\n");

    check_block(&x);


    printf("\n========== TEST 13: MULTIPLE ARENAS ==========\n");

    void *large1 = my_malloc(600000);
    void *large2 = my_malloc(600000);

    printf("large1 = %p\n", large1);
    printf("large2 = %p\n", large2);

    if (large1 != NULL && large2 != NULL)
        printf("PASS: multiple arenas working\n");
    else
        printf("FAIL: multiple arenas\n");


    printf("\n========== TEST 14: DOUBLE FREE ==========\n");

    void *df = my_malloc(100);

    my_free(df);
    printf("First free done\n");

    my_free(df);
    printf("Second free done\n");

    printf("PASS: allocator survived double free\n");


    printf("\n========== ALL TESTS COMPLETE ==========\n");

    return 0;
}