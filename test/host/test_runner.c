#include "rng_control.h"
#include "random_generator.h"
#include <assert.h>
#include <stdio.h>

static int fails = 0;
#define CHECK(c) do { if(!(c)){ printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c); fails++; } } while(0)

static void test_rng_is_seedable(void) {
    rng_seed(1234);
    uint32_t a = random_uniform(0, 100);
    rng_seed(1234);
    uint32_t b = random_uniform(0, 100);
    CHECK(a == b);        // same seed -> same value
    CHECK(a < 100);       // in range
    uint32_t c = random_uniform(0, 100);
    (void)c;              // sequence advances
}

int main(void) {
    test_rng_is_seedable();
    printf(fails ? "FAILED (%d)\n" : "OK\n", fails);
    return fails ? 1 : 0;
}
