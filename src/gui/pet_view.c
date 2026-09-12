#include <furi.h>
#include <stdio.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>

#include "pet_view.h"
#include "game_decoder.h"
#include "../flipper_structs.h"

typedef struct {
    uint8_t stage, alignment, display_state;
    uint32_t hunger, happiness, health;
    uint8_t poop, sick, night, call;
    uint32_t frame;
    uint8_t selected;
} PetModel;

static const char *ACTION_NAMES[PET_ACTION_COUNT] =
    {"Feed", "Play", "Clean", "Med", "Scold", "Lights"};

/* 7x6 heart bitmap for the meters. */
static const uint8_t HEART[6] = {
    0x36, // 0110110
    0x7F, // 1111111
    0x7F, // 1111111
    0x3E, // 0111110
    0x1C, // 0011100
    0x08, // 0001000
};
static bool heart_on(int r, int c) { return (r >= 0 && r < 6 && c >= 0 && c < 7) && ((HEART[r] >> (6 - c)) & 1); }

static void draw_meter_heart(Canvas *c, int x, int y, bool filled) {
    for(int r = 0; r < 6; r++) {
        for(int col = 0; col < 7; col++) {
            if(!heart_on(r, col)) continue;
            if(filled) {
                canvas_draw_dot(c, x + col, y + r);
            } else if(!heart_on(r - 1, col) || !heart_on(r + 1, col) ||
                      !heart_on(r, col - 1) || !heart_on(r, col + 1)) {
                canvas_draw_dot(c, x + col, y + r); // outline for an empty heart
            }
        }
    }
}

static void draw_meter_row(Canvas *c, int y, const char *label, uint32_t value) {
    canvas_set_font(c, FontSecondary);
    canvas_draw_str(c, 66, y + 6, label);
    int fh = (int)((value * 4 + 50) / 100);
    if(fh < 0) fh = 0;
    if(fh > 4) fh = 4;
    for(int i = 0; i < 4; i++) draw_meter_heart(c, 82 + i * 8, y, i < fh);
}

/* Small rising heart for the play overlay. */
static void draw_mini_heart(Canvas *c, int x, int y) {
    canvas_draw_dot(c, x, y);     canvas_draw_dot(c, x + 2, y);
    canvas_draw_dot(c, x - 1, y + 1); canvas_draw_dot(c, x + 1, y + 1); canvas_draw_dot(c, x + 3, y + 1);
    canvas_draw_dot(c, x, y + 2);  canvas_draw_dot(c, x + 1, y + 2); canvas_draw_dot(c, x + 2, y + 2);
    canvas_draw_dot(c, x + 1, y + 3);
}

static void draw_state_overlay(Canvas *c, uint8_t display_state, uint32_t frame) {
    int p = (int)(frame & 1u);
    switch(display_state) {
        case DISP_SLEEPING:
            canvas_set_font(c, FontSecondary);
            canvas_draw_str(c, 40, 12 - p * 2, "z");
            canvas_draw_str(c, 46, 8 - p * 2, "Z");
            break;
        case DISP_SICK:
            canvas_draw_disc(c, 46, 14 + p * 2, 1); // sweat drop
            break;
        case DISP_PLAYING:
            draw_mini_heart(c, 42, 8 - p * 2);
            break;
        case DISP_EATING:
            canvas_draw_dot(c, 8 + p, 34); // crumbs
            canvas_draw_dot(c, 12, 36 - p);
            canvas_draw_dot(c, 6, 37);
            break;
        case DISP_EVOLVING: {
            const int sx[4] = {6, 58, 10, 54};
            const int sy[4] = {6, 8, 50, 46};
            for(int i = 0; i < 4; i++) {
                int x = sx[i], y = sy[i] + (p ? 0 : 1);
                canvas_draw_line(c, x - 2, y, x + 2, y);
                canvas_draw_line(c, x, y - 2, x, y + 2);
            }
            break;
        }
        default:
            break;
    }
}

static void draw_poop(Canvas *c, int x, int y) {
    canvas_draw_disc(c, x, y, 2);
    canvas_draw_dot(c, x - 1, y - 2);
    canvas_draw_dot(c, x + 1, y - 3);
    canvas_draw_dot(c, x, y - 4);
}

static void pet_draw_callback(Canvas *canvas, void *model) {
    PetModel *m = model;
    canvas_clear(canvas);

    // motion by state
    int dx = 0, dy = 0;
    int p = (int)(m->frame & 1u);
    switch(m->display_state) {
        case DISP_IDLE:    dy = p ? 0 : 1; break;
        case DISP_SICK:    dx = p ? 1 : -1; break;
        case DISP_PLAYING: dy = p ? -2 : 0; break;
        case DISP_EATING:  dx = p; break;
        default: break;
    }
    const Icon *icon = decode_image_for(m->stage, m->alignment, m->display_state, m->frame);
    canvas_draw_icon(canvas, 2 + dx, 2 + dy, icon);
    draw_state_overlay(canvas, m->display_state, m->frame);
    if(m->poop) draw_poop(canvas, 52, 56);
    if(m->call) { canvas_set_font(canvas, FontPrimary); canvas_draw_str(canvas, 34, 12, "!"); }

    // heart meters
    draw_meter_row(canvas, 1, "Hu", m->hunger);
    draw_meter_row(canvas, 13, "Jo", m->happiness);
    draw_meter_row(canvas, 25, "Hp", m->health);

    // action selector + hint
    char buf[20];
    snprintf(buf, sizeof(buf), "< %s >", ACTION_NAMES[m->selected % PET_ACTION_COUNT]);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 66, 51, buf);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 66, 62, "OK  ^stat vset");
}

static bool pet_input_callback(InputEvent *event, void *context) {
    struct ApplicationContext *app = (struct ApplicationContext *)context;
    if(event->type != InputTypeShort) return false;
    switch(event->key) {
        case InputKeyLeft:
        case InputKeyRight: {
            bool right = (event->key == InputKeyRight);
            with_view_model(
                app->pet_view, PetModel * m,
                {
                    m->selected = right
                        ? (uint8_t)((m->selected + 1) % PET_ACTION_COUNT)
                        : (uint8_t)((m->selected + PET_ACTION_COUNT - 1) % PET_ACTION_COUNT);
                },
                true);
            return true;
        }
        case InputKeyOk: {
            uint8_t sel = 0;
            with_view_model(app->pet_view, PetModel * m, { sel = m->selected; }, false);
            view_dispatcher_send_custom_event(app->view_dispatcher, sel);
            return true;
        }
        case InputKeyUp:
            view_dispatcher_send_custom_event(app->view_dispatcher, PET_EVT_STATS);
            return true;
        case InputKeyDown:
            view_dispatcher_send_custom_event(app->view_dispatcher, PET_EVT_SETTINGS);
            return true;
        default:
            return false;
    }
}

View *pet_view_alloc(void *context) {
    View *view = view_alloc();
    view_set_context(view, context);
    view_set_draw_callback(view, pet_draw_callback);
    view_set_input_callback(view, pet_input_callback);
    view_allocate_model(view, ViewModelTypeLocking, sizeof(PetModel));
    with_view_model(view, PetModel * m, { m->selected = 0; m->stage = EGG; }, true);
    return view;
}

void pet_view_free(View *view) {
    view_free(view);
}

void pet_view_update(View *view, const struct GameState *gs, bool night) {
    const struct PersistentGameState *p = &gs->persistent;
    with_view_model(
        view, PetModel * m,
        {
            m->stage = p->stage;
            m->alignment = p->alignment;
            m->display_state = gs->display_state;
            m->hunger = p->hunger;
            m->happiness = p->happiness;
            m->health = p->health;
            m->poop = p->poop;
            m->sick = p->sick;
            m->night = night ? 1 : 0;
            m->call = p->attention_call;
            m->frame = gs->next_animation_index;
        },
        true);
}
