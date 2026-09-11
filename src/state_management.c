#include <furi.h>
#include "state_management.h"
#include "constants.h"
#include "game_model.h"
#include "game_logic.h"
#include "states.h"
#include "clock.h"
#include "save_restore.h"

GameEventFlags init_state(struct GameState *gs) {
    if(!load_state_from_file(&gs->persistent)) {
        game_state_init(gs, game_now());
        return EVT_NONE;
    }
    // Loaded: transient fields are not persisted, initialise them, then
    // fast-forward the simulation to now.
    gs->next_animation_index = 0;
    gs->display_state = DISP_IDLE;
    return advance_state(gs, game_now());
}

void persist_state(struct GameState *gs) {
    if(!save_state_to_file(&gs->persistent)) {
        furi_crash("Unable to save state to storage");
    }
}

void reset_state(struct GameState *gs) {
    game_state_init(gs, game_now());
}

GameEventFlags tick_state(struct GameState *gs) {
    return advance_state(gs, game_now());
}

GameEventFlags do_action(struct GameState *gs, enum ThreadsMessageType type) {
    uint32_t now = game_now();
    switch(type) {
        case PROCESS_FEED:     return do_feed(gs, now);
        case PROCESS_PLAY:     return do_play(gs, now);
        case PROCESS_CLEAN:    return do_clean(gs, now);
        case PROCESS_MEDICINE: return do_medicine(gs, now);
        case PROCESS_SCOLD:    return do_scold(gs, now);
        case TOGGLE_LIGHTS:    return do_lights(gs, now);
        default:               return EVT_NONE;
    }
}

bool state_is_night_now(void) {
    return is_night(game_now());
}

static const char *care_word(int32_t care) {
    if(care >= 80) return "Thriving";
    if(care >= 60) return "Content";
    if(care >= 40) return "OK";
    if(care >= 20) return "Poor";
    return "Neglected";
}

void get_state_str(const struct GameState *gs, char *str, size_t size) {
    const struct PersistentGameState *p = &gs->persistent;
    uint32_t now = game_now();
    uint32_t age_days = (now > p->birth_timestamp) ? (now - p->birth_timestamp) / 86400u : 0;
    if(p->stage == ADULT && p->alignment != ALIGN_NONE) {
        snprintf(str, size,
                 "%s %s\nAge: %lud\nDiscipline: %lu\nCare: %s",
                 ALIGNMENT_STRING[p->alignment],
                 LIFE_STAGE_STRING[p->stage],
                 (unsigned long)age_days,
                 (unsigned long)p->discipline,
                 care_word(p->care_score));
    } else {
        snprintf(str, size,
                 "Stage: %s\nAge: %lud\nDiscipline: %lu\nCare: %s",
                 LIFE_STAGE_STRING[p->stage],
                 (unsigned long)age_days,
                 (unsigned long)p->discipline,
                 care_word(p->care_score));
    }
}
