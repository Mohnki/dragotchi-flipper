#include <stdio.h>
#include "hunt.h"
#include "tuning.h"
#include "random_generator.h"

bool forage_ready(const struct GameState *gs, uint32_t now) {
    uint32_t last = gs->persistent.last_forage_time;
    return now >= last && (now - last) >= FORAGE_COOLDOWN;
}

static uint32_t lerp_pct(uint32_t lo, uint32_t hi, uint8_t a /*0-100*/) {
    return (uint32_t)((int)lo + ((int)hi - (int)lo) * (int)a / 100);
}

struct Catch catch_roll(uint8_t activity, uint8_t band) {
    if(activity > 100) activity = 100;
    uint32_t prey = lerp_pct(PREY_PCT_LO, PREY_PCT_HI, activity);
    uint32_t treasure = lerp_pct(TREASURE_PCT_LO, TREASURE_PCT_HI, activity);

    struct Catch c = {.band = band, .tier = TIER_SMALL, .value = 0};
    uint32_t r = random_uniform(0, 100);
    if(r < prey) c.category = CATCH_PREY;
    else if(r < prey + treasure) c.category = CATCH_TREASURE;
    else c.category = CATCH_EGG;

    uint32_t t = random_uniform(0, 100) + activity / 2; // activity nudges tier up
    uint8_t tier = (t >= 120) ? TIER_LARGE : (t >= 70) ? TIER_MED : TIER_SMALL;

    if(c.category == CATCH_PREY) {
        c.tier = tier;
        c.value = tier == TIER_LARGE ? PREY_FOOD_LARGE : tier == TIER_MED ? PREY_FOOD_MED : PREY_FOOD_SMALL;
    } else if(c.category == CATCH_TREASURE) {
        c.tier = tier;
        c.value = tier == TIER_LARGE ? TREASURE_LARGE : tier == TIER_MED ? TREASURE_MED : TREASURE_SMALL;
    } else { // EGG: tier = rarity (0 common, 1 rare); rarer with activity
        c.tier = (random_uniform(0, 100) < (20u + activity / 2)) ? 1 : 0;
        c.value = 0;
    }
    return c;
}

void apply_catch(struct GameState *gs, struct Catch c, uint32_t now) {
    struct PersistentGameState *p = &gs->persistent;
    switch(c.category) {
        case CATCH_PREY:
            p->hunger = (p->hunger + c.value > MAX_HU) ? MAX_HU : p->hunger + c.value;
            p->happiness = (p->happiness + 5 > MAX_HAPPINESS) ? MAX_HAPPINESS : p->happiness + 5;
            p->prey_caught++;
            gs->display_state = DISP_EATING;
            break;
        case CATCH_TREASURE:
            p->hoard += c.value;
            if(c.tier == TIER_LARGE) p->treasure_large++;
            else if(c.tier == TIER_MED) p->treasure_med++;
            else p->treasure_small++;
            gs->display_state = DISP_PLAYING;
            break;
        case CATCH_EGG:
            if(c.tier) p->eggs_rare++;
            else p->eggs_common++;
            p->eggs_caught++;
            gs->display_state = DISP_PLAYING;
            break;
    }
    p->last_forage_time = now;
}

uint8_t signal_storm_activity(uint8_t base_activity, uint8_t wifi_count) {
    uint32_t bonus = (uint32_t)wifi_count * STORM_PER_AP;
    if(bonus > STORM_MAX_BONUS) bonus = STORM_MAX_BONUS;
    uint32_t a = (uint32_t)base_activity + bonus;
    if(a > 100) a = 100;
    return (uint8_t)a;
}

bool signal_storm_active(uint8_t wifi_count) {
    return wifi_count >= STORM_MIN_APS;
}

const char *band_name(uint8_t band) {
    static const char *n[HUNT_BANDS] = {"315", "433", "868", "915"};
    return n[band % HUNT_BANDS];
}

int catch_describe(struct Catch c, char *buf, size_t n) {
    static const char *flavor[HUNT_BANDS] = {"gremlin", "wyrm", "drake", "serpent"};
    const char *fl = flavor[c.band % HUNT_BANDS];
    switch(c.category) {
        case CATCH_PREY:     return snprintf(buf, n, "%s %s +%u food", band_name(c.band), fl, c.value);
        case CATCH_TREASURE: return snprintf(buf, n, "Treasure +%u", c.value);
        default:             return snprintf(buf, n, "%s egg!", c.tier ? "Rare" : "Common");
    }
}
