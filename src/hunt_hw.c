#include "hunt_hw.h"
#include <furi.h>
#include <furi_hal_random.h>
#include "tuning.h"

/* v0.2: a safe, non-blocking activity source so hunting is stable and fun.
 * True sub-GHz RSSI "airwave" sensing is a separate isolated spike — the raw
 * radio HAL isn't exported to FAPs (furi_hal_subghz_init/cc1101_set_frequency
 * are unavailable), and the subghz_devices layer faults from a plain app, so it
 * needs debugging in a standalone test FAP before it goes near the game. */
void hunt_sense(uint8_t *activity_out, uint8_t *band_out) {
    uint32_t r = furi_hal_random_get();
    *activity_out = (uint8_t)(r % 101u);
    *band_out = (uint8_t)((r >> 8) % HUNT_BANDS);
}
