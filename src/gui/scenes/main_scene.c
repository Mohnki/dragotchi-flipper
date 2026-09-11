#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#include "main_scene.h"
#include "scenes.h"
#include "../pet_view.h"
#include "../../flipper_structs.h"
#include "../../constants.h"
#include "../../state_management.h"

void scene_main_on_enter(void *ctx) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "scene_main_on_enter");
    pet_view_update(context->pet_view, context->game_state, state_is_night_now());
    view_dispatcher_switch_to_view(context->view_dispatcher, scene_main);
}

bool scene_main_on_event(void *ctx, SceneManagerEvent event) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    switch(event.type) {
        case SceneManagerEventTypeBack:
            FURI_LOG_T(LOG_TAG, "scene_main: back -> exit");
            scene_manager_stop(context->scene_manager);
            view_dispatcher_stop(context->view_dispatcher);
            return true;
        case SceneManagerEventTypeTick:
            pet_view_update(context->pet_view, context->game_state, state_is_night_now());
            return true;
        case SceneManagerEventTypeCustom: {
            uint32_t e = event.event;
            if(e < PET_ACTION_COUNT) {
                struct ThreadsMessage msg = {.type = (enum ThreadsMessageType)(PROCESS_FEED + e)};
                furi_message_queue_put(context->threads_message_queue, &msg, FuriWaitForever);
            } else if(e == PET_EVT_STATS) {
                scene_manager_next_scene(context->scene_manager, scene_status);
            } else if(e == PET_EVT_SETTINGS) {
                scene_manager_next_scene(context->scene_manager, scene_settings);
            }
            return true;
        }
        default:
            break;
    }
    return false;
}

void scene_main_on_exit(void *ctx) {
    UNUSED(ctx);
    FURI_LOG_T(LOG_TAG, "scene_main_on_exit");
}
