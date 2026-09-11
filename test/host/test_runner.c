#include "tests.h"
#include <stdio.h>
int g_fails = 0;
int main(void) {
    run_rng_tests();
    run_model_tests();
    printf(g_fails ? "FAILED (%d)\n" : "ALL OK\n", g_fails);
    return g_fails ? 1 : 0;
}
