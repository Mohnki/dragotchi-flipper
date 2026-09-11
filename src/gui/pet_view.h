#ifndef __pet_view_h__
#define __pet_view_h__
#include <gui/view.h>
#include "../game_structs.h"

#define PET_ACTION_COUNT 6
/* Custom events sent from the pet view's input handler. Action indices 0..5
 * map to ThreadsMessageType PROCESS_FEED + index. */
#define PET_EVT_STATS 100u
#define PET_EVT_SETTINGS 101u

/* @context must be the ApplicationContext (used by the input callback). */
View *pet_view_alloc(void *context);
void pet_view_free(View *);
/* Refresh the drawn snapshot from the game state. */
void pet_view_update(View *, const struct GameState *, bool night);
#endif
