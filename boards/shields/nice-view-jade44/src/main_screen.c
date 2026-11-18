#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/usb.h>
#include <zmk/ble.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>
#include <zmk/wpm.h>

#include "include/main_screen.h"
#include "widgets/include/layer.h"
#include "widgets/include/battery.h"

static struct zmk_screen_widget screen_widget;

/* Okay, here's my best understanding of what these two macros are doing:
 * ZMK_DISPLAY_WIDGET_LISTENER(listener, state_type, cb, state_func)
 *   listener: Used as a prefix for what the macro defines, most importantly listener_init, which we must call.
 *   state_type: A type that we have defined. It will be used in the various callbacks and handlers.
 *   cb: A function we define that zmk will call. ZMK will pass an instance of state_type.
 *   state_func: A function we define that zmk will call. It takes a zmk_event_t and returns an instance of state_type.
 *
 * ZMK_SUBSCRIPTION(listener, ev_type)
 *   listener: Again a prefix.
 *   ev_type: Finally we get to "what event are we listening for?". Needs a ZMK_EVENT_DECLARE(ev_type) somewhere.
 *
 * So with these two together, zmk will watch for when ev_type is fired, and it will call state_func,
 * which takes the event and returns a filled-in instance of state_type, which is then handed to cb. The cb function is
 * responsible for acting on the changes. Oh, and we do need to call listener_init, which is defined for us.
 *
 * Not a huge fan of this level of macro complexity, but that's just how it goes when we're trying to make things
 * work auto-magically in C.
 */

static void set_battery_status(struct zmk_screen_widget *widget,
                               struct battery_state new_battery_state) {
    widget->state.battery = new_battery_state;

    draw_top(widget->obj, &widget->state.battery);
}

static void battery_status_update_cb(struct battery_state state) {
    set_battery_status(&screen_widget, state);
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);

/* LAYER WIDGET */

static void set_layer_status(struct zmk_screen_widget *widget, struct layer_state new_layer_state) {
    widget->state.layer = new_layer_state;

    draw_layer_status(widget->obj, &widget->state.layer);
}

static void layer_update_cb(struct layer_state state) {
    set_layer_status(&screen_widget, state);
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_state,
                            layer_update_cb, layer_get_state)

ZMK_SUBSCRIPTION(widget_layer_status, zmk_layer_state_changed);

int zmk_screen_widget_init(struct zmk_screen_widget *widget, lv_obj_t *screen) {
    widget->obj = lv_obj_create(screen);
    lv_obj_set_size(widget->obj, 160, 68);
    
    lv_obj_t *top = lv_canvas_create(widget->obj);
    lv_obj_align(top, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_canvas_set_buffer(top, widget->top_buffer, CANVAS_SIZE, CANVAS_SIZE, CANVAS_COLOR_FORMAT);
    
    lv_obj_t *middle = lv_canvas_create(widget->obj);
    lv_obj_align(middle, LV_ALIGN_TOP_LEFT, 24, 0);
    lv_canvas_set_buffer(middle, widget->mid_buffer, CANVAS_SIZE, CANVAS_SIZE, CANVAS_COLOR_FORMAT);
    
    lv_obj_t *bottom = lv_canvas_create(widget->obj);
    lv_obj_align(bottom, LV_ALIGN_TOP_LEFT, -44, 0);
    lv_canvas_set_buffer(bottom, widget->low_buffer, CANVAS_SIZE, CANVAS_SIZE, CANVAS_COLOR_FORMAT);

    /* These _init functions defined by the ZMK_DISPLAY_WIDGET_LISTENER macro.
     * We need to call them, but we don't need to care about them.
     */
    widget_battery_status_init();
    widget_layer_status_init();

    return 0;
}

lv_obj_t *zmk_screen_widget_obj(struct zmk_screen_widget *widget) { return widget->obj; }