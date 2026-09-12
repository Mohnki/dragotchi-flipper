#include "tests.h"
#include <stdio.h>
int g_fails = 0;
int main(void) {
    run_rng_tests();
    run_model_tests();
    run_needs_tests();
    run_states_tests();
    run_discipline_tests();
    run_evolution_tests();
    run_integration_tests();
    run_save_tests();
    run_overnight_tests();
    run_hunt_tests();
    run_hunt_logic_tests();
    run_economy_tests();
    printf(g_fails ? "FAILED (%d)\n" : "ALL OK\n", g_fails);
    return g_fails ? 1 : 0;
}
