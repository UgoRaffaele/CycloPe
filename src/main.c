#include <pebble.h>
#include "main.h"

static Window *s_main_window;
static TextLayer *s_output_layer, *s_status_layer;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    
    // Toggle running state
    AppWorkerResult result;
    
    // Check to see if the worker is currently active
    if (app_worker_is_running()) {
        
        result = app_worker_kill();

        if (result == APP_WORKER_RESULT_SUCCESS) {
            text_layer_set_text(s_status_layer, "Worker stopped!");
        } else {
            text_layer_set_text(s_status_layer, "Error killing worker!");
        }
        
    } else {
        
        result = app_worker_launch();

        if (result == APP_WORKER_RESULT_SUCCESS) {
            text_layer_set_text(s_status_layer, "Worker launched!");
        } else {
            text_layer_set_text(s_status_layer, "Error launching worker!");
        }
        
    }

    APP_LOG(APP_LOG_LEVEL_INFO, "worker result: %d", result);
    
}

static void main_window_load(Window *window) {
    
    // Create Window's child Layers here
    Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer);

    s_output_layer = text_layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
    text_layer_set_text(s_output_layer, "Use SELECT to start/stop the background worker.");
    text_layer_set_text_alignment(s_output_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_output_layer));

    s_status_layer = text_layer_create(GRect(0, 135, window_bounds.size.w, 30));
    if (app_worker_is_running()) {
        text_layer_set_text(s_status_layer, "Worker running!");
    } else {
        text_layer_set_text(s_status_layer, "Worker halted!");
    }
    text_layer_set_text_alignment(s_status_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_status_layer));
    
}

static void main_window_unload(Window *window) {
    
    // Destroy Window's child Layers here
    text_layer_destroy(s_output_layer);
    text_layer_destroy(s_status_layer);
    
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void init() {
    // Create main Window
    s_main_window = window_create();
    window_set_click_config_provider(s_main_window, click_config_provider);
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload,
    });
    window_stack_push(s_main_window, true);
}

static void deinit() {
    // Destroy main Window
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
    return 0;
}