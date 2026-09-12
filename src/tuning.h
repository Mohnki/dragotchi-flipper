#ifndef __tuning_h__
#define __tuning_h__
/* All game-balance constants. Host-safe: no device headers.
 * Two profiles: NORMAL (real timescale) and DEBUG (compressed, for testing).
 * DEBUG build: ufbt --extra-define=DEBUG launch APPSRC=dragotchi */
#include <stdint.h>

/* Meters */
#define MAX_HU 100
#define MAX_HAPPINESS 100
#define MAX_HP 100

/* Care score */
#define CARE_START 50
#define CARE_MIN 0
#define CARE_MAX 100
#define CARE_WHITE 66   // >= -> white dragon
#define CARE_GREY 33    // >= -> grey; below -> black
#define CARE_IMMORTAL 90 // >= while adult -> never dies of old age

/* Care reward/penalty magnitudes */
#define CARE_FEED_HUNGRY 3
#define CARE_PLAY_SAD 3
#define CARE_CLEAN 4
#define CARE_HEAL 4
#define CARE_LIGHTS_OUT 2
#define CARE_SCOLD_CORRECT 3
#define CARE_METER_ZERO 6
#define CARE_POOP_IGNORED 5
#define CARE_SICK_IGNORED 6
#define CARE_SLEEP_DISTURBED 3
#define CARE_OVERFEED 2
#define CARE_SCOLD_WRONG 3

/* Poop / sickness caps */
#define MAX_POOP 3

/* Sleep window (RTC hours) */
#define NIGHT_START 20
#define NIGHT_END 8

#ifdef DEBUG
/* -------- compressed timescale (seconds) -------- */
#define HU_DECAY_FREQ 3
#define HAP_DECAY_FREQ 4
#define HP_CHECK_FREQ 3
#define POOP_FREQ 6
#define SICK_CHECK_FREQ 8
#define OLD_AGE_CHECK_FREQ 5
#define POOP_TOLERANCE 10
#define SICK_TOLERANCE 12
#define AGE_HATCHLING 5
#define AGE_WYRMLING 30
#define AGE_DRAKE 60
#define AGE_ADULT 120
#define AGE_ELDER 300
#else
/* -------- normal timescale (seconds) -------- */
#define HU_DECAY_FREQ 600      // 10 min
#define HAP_DECAY_FREQ 900     // 15 min
#define HP_CHECK_FREQ 300      // 5 min
#define POOP_FREQ 3600         // ~ a few per day
#define SICK_CHECK_FREQ 1800   // 30 min
#define OLD_AGE_CHECK_FREQ 3600
#define POOP_TOLERANCE 3600    // 1h uncleaned = mistake
#define SICK_TOLERANCE 3600    // 1h untreated = mistake
#define AGE_HATCHLING 1800     // 30 min: egg hatches
#define AGE_WYRMLING 86400     // day 1
#define AGE_DRAKE 216000       // day 2.5
#define AGE_ADULT 388800       // day 4.5
#define AGE_ELDER 864000       // day 10: earliest old-age death
#endif

/* Probabilities (0..100) and magnitudes (shared across profiles) */
#define HU_DECAY_PROB 40
#define HU_DECAY_MIN 1
#define HU_DECAY_MAX 4
#define FEED_MIN 20
#define FEED_MAX 40

#define HAP_DECAY_PROB 40
#define HAP_DECAY_MIN 1
#define HAP_DECAY_MAX 4
#define PLAY_MIN 20
#define PLAY_MAX 40

#define HP_DRAIN_MIN 1
#define HP_DRAIN_MAX 5
#define MEDICINE_MIN 30
#define MEDICINE_MAX 60

#define POOP_PROB 50

#define SICK_BASE_PROB 5
#define SICK_HUNGRY_BONUS 20
#define SICK_DIRTY_BONUS 10

/* Old-age death: per-check probability scales with how far below the
 * immortality bar the care score sits. */
#define OLD_AGE_BASE_PROB 8

/* Discipline / attention calls */
#define DISCIPLINE_START 50
#define ATTENTION_CALL_FREQ 1200
#define ATTENTION_CALL_BASE_PROB 10
#define DISCIPLINE_SCOLD_GAIN 8
#define DISCIPLINE_SPOIL_LOSS 5

#endif
