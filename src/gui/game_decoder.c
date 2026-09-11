#include <furi.h>
#include "game_decoder.h"
#include "dragotchi_icons.h"

/* Placeholder sprite sets (reused from the fork) until Phase 5 dragon art.
 * Order matches enum LifeStage: EGG, HATCHLING, WYRMLING, DRAKE, ADULT, DEAD. */
static const Icon *life_stages[LIFE_STAGES_NUM][6] = {
    {&I_egg_00_60x60, &I_egg_01_60x60},
    {&I_baby_00_60x60, &I_baby_01_60x60, &I_baby_02_60x60,
     &I_baby_03_60x60, &I_baby_04_60x60, &I_baby_05_60x60},
    {&I_child_00_60x60, &I_child_01_60x60},
    {&I_teen_00_60x60, &I_teen_01_60x60},
    {&I_adult_00_60x60, &I_adult_01_60x60},
    {&I_dead_00_60x60, &I_dead_01_60x60}};

static const uint8_t stage_frames[LIFE_STAGES_NUM] = {2, 6, 2, 2, 2, 2};

const Icon *decode_image_for(uint8_t stage, uint8_t alignment, uint8_t display_state, uint32_t frame) {
    (void)alignment;   // Phase 5: white/grey/black adult art
    (void)display_state; // Phase 5: eat/play/sick/sleep frames
    if(stage >= LIFE_STAGES_NUM) stage = EGG;
    return life_stages[stage][frame % stage_frames[stage]];
}

const Icon *decode_image(const struct GameState *game_state) {
    return decode_image_for(game_state->persistent.stage,
                            game_state->persistent.alignment,
                            game_state->display_state,
                            game_state->next_animation_index);
}
