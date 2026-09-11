#include <furi.h>
#include "game_decoder.h"
#include "dragotchi_icons.h"

const Icon *decode_image_for(uint8_t stage, uint8_t alignment, uint8_t display_state, uint32_t frame) {
    (void)display_state; // eat/play/sick/sleep frames: future art
    uint32_t f = frame % 2u;
    switch(stage) {
        case EGG:       return f ? &I_egg_01_60x60   : &I_egg_00_60x60;
        case HATCHLING: return f ? &I_hatch_01_60x60 : &I_hatch_00_60x60;
        case WYRMLING:  return f ? &I_wyrm_01_60x60  : &I_wyrm_00_60x60;
        case DRAKE:     return f ? &I_drake_01_60x60 : &I_drake_00_60x60;
        case ADULT:
            if(alignment == ALIGN_WHITE) return f ? &I_adult_white_01_60x60 : &I_adult_white_00_60x60;
            if(alignment == ALIGN_BLACK) return f ? &I_adult_black_01_60x60 : &I_adult_black_00_60x60;
            return f ? &I_adult_grey_01_60x60 : &I_adult_grey_00_60x60;
        case DEAD:      return f ? &I_dead_01_60x60  : &I_dead_00_60x60;
        default:        return &I_egg_00_60x60;
    }
}

const Icon *decode_image(const struct GameState *game_state) {
    return decode_image_for(game_state->persistent.stage,
                            game_state->persistent.alignment,
                            game_state->display_state,
                            game_state->next_animation_index);
}
