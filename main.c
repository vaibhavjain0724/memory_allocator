#include "allocator.h"
#include <stdio.h>
#include <stddef.h>

int main() {

    printf("========== TEST 1: BASIC ALLOCATION ==========\n");

    void *a = my_malloc(100);

    printf("a = %p\n", a);
    check_block(a);


    printf("\n========== TEST 2: ALIGNMENT ==========\n");

    void *b = my_malloc(9);

    printf("b = %p\n", b);
    check_block(b);

    printf("Expected size: 16\n");


    printf("\n========== TEST 3: MULTIPLE ALLOCATIONS ==========\n");

    void *c = my_malloc(100);
    void *d = my_malloc(200);

    printf("c = %p\n", c);
    check_block(c);

    printf("d = %p\n", d);
    check_block(d);


    printf("\n========== TEST 4: FREE + REUSE ==========\n");

    printf("Freeing c...\n");
    my_free(c);

    printf("Allocating 80 bytes...\n");

    void *e = my_malloc(80);

    printf("e = %p\n", e);
    check_block(e);

    printf("c = %p\n", c);

    if (e == c)
        printf("PASS: freed block was reused\n");
    else
        printf("FAIL: block was not reused\n");


    printf("\n========== TEST 5: SPLITTING ==========\n");

    void *f = my_malloc(300);

    printf("f = %p\n", f);
    check_block(f);

    my_free(f);

    printf("Freed f.\n");

    void *g = my_malloc(100);

    printf("g = %p\n", g);
    check_block(g);

    printf("g = %p\n", g);
    printf("f = %p\n", f);

    if (g == f)
        printf("PASS: reused block\n");
    else
        printf("FAIL: did not reuse block\n");


    printf("\n========== TEST 6: COALESCING ==========\n");

    void *h = my_malloc(100);
    void *i = my_malloc(100);
    void *j = my_malloc(100);

    printf("h = %p\n", h);
    printf("i = %p\n", i);
    printf("j = %p\n", j);

    printf("\nFreeing i...\n");
    my_free(i);

    printf("Freeing h...\n");
    my_free(h);

    printf("Allocating 180 bytes...\n");

    void *k = my_malloc(180);

    printf("k = %p\n", k);
    check_block(k);

    if (k == h)
        printf("PASS: coalesced blocks were reused\n");
    else
        printf("FAIL: coalesced blocks were not reused\n");


    printf("\n========== TEST 7: LARGE ALLOCATION ==========\n");

    void *l = my_malloc(500000);

    printf("l = %p\n", l);

    if (l != NULL)
        printf("PASS: large allocation succeeded\n");
    else
        printf("FAIL: large allocation failed\n");


    printf("\n========== TEST 8: MULTIPLE ARENAS ==========\n");

    void *m = my_malloc(600000);

    printf("m = %p\n", m);

    void *n = my_malloc(600000);

    printf("n = %p\n", n);

    if (m != NULL && n != NULL)
        printf("PASS: allocator created/used another arena\n");
    else
        printf("FAIL: multiple arena allocation failed\n");


    printf("\n========== TEST 9: NULL FREE ==========\n");

    my_free(NULL);

    printf("PASS: my_free(NULL) did not crash\n");


    printf("\n========== ALL TESTS COMPLETE ==========\n");

    return 0;
}