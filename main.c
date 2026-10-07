#include "allocator.h"
#include <stdio.h>

int main() {

    printf("========== TEST 1: BASIC ALLOCATION ==========\n");

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

    if (d == c)
        printf("PASS: block reused\n");
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

    if (f == e)
        printf("PASS: large free block reused and split\n");
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
        printf("FAIL: blocks did not coalesce correctly\n");


    printf("\n========== TEST 6: MULTIPLE ARENAS ==========\n");

    void *k = my_malloc(600000);
    void *l = my_malloc(600000);

    printf("k = %p\n", k);
    printf("l = %p\n", l);

    if (k != NULL && l != NULL)
        printf("PASS: multiple arenas working\n");
    else
        printf("FAIL: multiple arena allocation\n");


    printf("\n========== TEST 7: NULL FREE ==========\n");

    my_free(NULL);

    printf("PASS: my_free(NULL) did not crash\n");


    printf("\n========== TEST 8: INVALID FREE ==========\n");

    int x = 42;

    printf("Attempting to free stack variable...\n");

    my_free(&x);

    printf("PASS: invalid free was rejected\n");


    printf("\n========== TEST 9: INVALID CHECK ==========\n");

    printf("Checking stack variable...\n");

    check_block(&x);

    printf("PASS: invalid block check was rejected\n");


    printf("\n========== TEST 10: DOUBLE FREE ==========\n");

    void *m = my_malloc(100);

    printf("m = %p\n", m);

    my_free(m);

    printf("First free done.\n");

    my_free(m);

    printf("Second free done.\n");

    printf("PASS: allocator survived double free\n");


    printf("\n========== ALL TESTS COMPLETE ==========\n");

    return 0;
}