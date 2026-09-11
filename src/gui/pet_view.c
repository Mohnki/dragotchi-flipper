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

static void draw_heart(Canvas *c, int x, int y) {
    canvas_draw_dot(c, x, y);     canvas_draw_dot(c, x + 2, y);
    canvas_draw_dot(c, x - 1, y + 1); canvas_draw_dot(c, x + 1, y + 1); canvas_draw_dot(c, x + 3, y + 1);
    canvas_draw_dot(c, x, y + 2);  canvas_draw_dot(c, x + 1, y + 2); canvas_draw_dot(c, x + 2, y + 2);
    canvas_draw_dot(c, x + 1, y + 3);
}

static void draw_state_overlay(Canvas *c, uint8_t display_state, uint32_t f) {
    int p = (int)(f & 1u);
    switch(display_state) {
        case DISP_SLEEPING:
            canvas_set_font(c, FontSecondary);
            canvas_draw_str(c, 40, 12 - p * 2, "z");
            canvas_draw_str(c, 46, 8 - p * 2, "Z");
            break;
        case DISP_SICK: {
            int dy = p * 2;
            canvas_draw_disc(c, 46, 14 + dy, 1);   // sweat drop, dripping
            break;
        }
        case DISP_PLAYING:
            draw_heart(c, 42, 8 - p * 2);          // heart rising
            break;
        case DISP_EATING:
            canvas_draw_dot(c, 8 + p, 34);         // crumbs near the snout
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

static void pet_draw_callback(Canvas *canvas, void *model) {
    PetModel *m = model;
    canvas_clear(canvas);

    // State-specific motion, then sprite, then animated overlay.
    int dx = 0, dy = 0;
    int p = (int)(m->frame & 1u);
    switch(m->display_state) {
        case DISP_IDLE:     dy = p ? 0 : 1; break;   // gentle bob
        case DISP_SICK:     dx = p ? 1 : -1; break;  // shivering
        case DISP_PLAYING:  dy = p ? -2 : 0; break;  // bouncing
        case DISP_EATING:   dx = p; break;           // chomping
        default:            break;                    // sleeping/dead: still
    }
    const Icon *icon = decode_image_for(m->stage, m->alignment, m->display_state, m->frame);
    canvas_draw_icon(canvas, 2 + dx, 2 + dy, icon);
    draw_state_overlay(canvas, m->display_state, m->frame);

    char buf[20];
    canvas_set_font(canvas, FontSecondary);
    snprintf(buf, sizeof(buf), "Hu %lu", (unsigned long)m->hunger);
    canvas_draw_str(canvas, 66, 9, buf);
    snprintf(buf, sizeof(buf), "Jo %lu", (unsigned long)m->happiness);
    canvas_draw_str(canvas, 66, 19, buf);
    snprintf(buf, sizeof(buf), "Hp %lu", (unsigned long)m->health);
    canvas_draw_str(canvas, 66, 29, buf);

    char flags[24];
    snprintf(flags, sizeof(flags), "%s%s%s%s",
             m->sick ? "SICK " : "",
             m->poop ? "DIRTY " : "",
             (m->display_state == DISP_SLEEPING) ? "Zzz " : "",
             m->call ? "!" : "");
    canvas_draw_str(canvas, 66, 39, flags);

    snprintf(buf, sizeof(buf), "< %s >", ACTION_NAMES[m->selected % PET_ACTION_COUNT]);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 66, 52, buf);
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
            return false; // Back handled by navigation callback
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
