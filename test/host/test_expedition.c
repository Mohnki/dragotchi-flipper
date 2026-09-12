#include "tests.h"
#include "test_util.h"
#include "game_model.h"
#include "tuning.h"
void run_expedition_defaults_tests(void) {
    struct GameState s = {0};
    game_state_init(&s, 100);
    CHECK(s.persistent.on_expedition == 0);
    CHECK(s.persistent.expedition_minutes == 0);
    CHECK(s.journey_ready == 0);
}
