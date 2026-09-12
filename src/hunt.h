#ifndef __hunt_h__
#define __hunt_h__
#include <stddef.h>
#include "game_structs.h"
/* Cooldown gate. */
bool forage_ready(const struct GameState *, uint32_t now);
/* Roll a catch given RF activity (0-100) and the hottest band. Uses global RNG. */
struct Catch catch_roll(uint8_t activity, uint8_t band);
/* Apply a catch's effects and stamp the forage cooldown. */
void apply_catch(struct GameState *, struct Catch, uint32_t now);
const char *band_name(uint8_t band);           /* "315".."915" */
int catch_describe(struct Catch, char *buf, size_t n);
#endif
