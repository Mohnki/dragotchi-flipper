#include "tests.h"
#include "test_util.h"
#include "game_model.h"
#include "tuning.h"

void run_hunt_tests(void) {
    struct GameState s = {0};
    game_state_init(&s, 1000);
    CHECK(s.persistent.hoard == 0);
    CHECK(s.persistent.eggs_common == 0 && s.persistent.eggs_rare == 0);
    CHECK(s.persistent.last_forage_time == 0);
}
