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

#include "hunt.h"
#include "rng_control.h"
void run_hunt_logic_tests(void) {
    struct GameState g = {0}; game_state_init(&g, 0);
    g.persistent.last_forage_time = 1000;
    CHECK(forage_ready(&g, 1000 + FORAGE_COOLDOWN) == true);
    CHECK(forage_ready(&g, 1000 + FORAGE_COOLDOWN - 1) == false);

    rng_seed(5);
    struct Catch c = catch_roll(50, 1);
    CHECK(c.category <= CATCH_EGG);
    CHECK(c.band == 1);

    rng_seed(9); int hi = 0; for(int i=0;i<400;i++){ if(catch_roll(100,1).category!=CATCH_PREY) hi++; }
    rng_seed(9); int lo = 0; for(int i=0;i<400;i++){ if(catch_roll(0,1).category!=CATCH_PREY) lo++; }
    CHECK(hi > lo);

    game_state_init(&g, 0); g.persistent.hunger = 10;
    apply_catch(&g, (struct Catch){CATCH_PREY, TIER_SMALL, PREY_FOOD_SMALL, 1}, 500);
    CHECK(g.persistent.hunger == 10 + PREY_FOOD_SMALL);
    CHECK(g.persistent.last_forage_time == 500);
    apply_catch(&g, (struct Catch){CATCH_TREASURE, TIER_MED, TREASURE_MED, 1}, 600);
    CHECK(g.persistent.hoard == TREASURE_MED);
    apply_catch(&g, (struct Catch){CATCH_EGG, 1, 0, 1}, 700);
    CHECK(g.persistent.eggs_rare == 1);
}
