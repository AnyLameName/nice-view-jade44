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

#include <main_screen.h>
#include <battery.h>
#include <util.h>

static struct zmk_screen_widget *screen_widget;

/* Okay, here's my best understanding of what these two macros are doing:
 * ZMK_DISPLAY_WIDGET_LISTENER(listener, state_type, cb, state_func)
 *   listener: Used as a prefix for what the macro defines, most importantly listener_init, which we must call.
 *   state_type: A type that we have defined. It will be used in the various callbacks and handlers.
 *   cb: A function we define that zmk will call. ZMK will pass an instance of state_type.
 *   state_func: A function we define that zmk will call. It takes a zmk_event_t and returns an instance of state_type.
 *
 * ZMK_SUBSCRIPTION(listener, ev_type)
 *   listener: Again a prefix.
 *   ev_type: Finally we get to "what event are we listening for?". Needs a ZMK_EVENT_DECLARE(ev_type) somewhere in zmk.
 *
 * So with these two together, zmk will watch for when ev_type is fired, and it will call state_func,
 * which takes the event and returns a filled-in instance of state_type, which is then handed to cb. The cb function is
 * responsible for acting on the changes. Oh, and we do need to call listener_init, which is defined for us.
 *
 * Not a huge fan of this level of macro complexity, but that's just how it goes when we're trying to make things
 * work auto-magically in C.
 */

/* BATTERY WIDGET */

static void battery_status_update_cb(struct battery_state new_battery_state) {
    screen_widget->state.battery = new_battery_state;

    /* The new battery state is passed separately like this so it can be received
     * as a const pointer, but this raises the question of why it even needs to be
     * part of screen_widget at all.
     */
    draw_battery(screen_widget->obj, &screen_widget->state.battery);
}

ZMK_DISPLAY_WIDGET_LISTENER(battery_widget, struct battery_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(battery_widget, zmk_battery_state_changed);

// Set up the canvas buffers and initialize all widgets
int zmk_screen_widget_init(struct zmk_screen_widget *widget, lv_obj_t *screen) {
    screen_widget = widget;

    screen_widget->obj = lv_obj_create(screen);
    lv_obj_set_size(screen_widget->obj, SCREEN_HEIGHT, SCREEN_WIDTH);
    
    lv_obj_t *main = lv_canvas_create(screen_widget->obj);
    lv_obj_align(top, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_canvas_set_buffer(top, screen_widget->top_buffer, CANVAS_SIZE, CANVAS_SIZE, CANVAS_COLOR_FORMAT);

    /* These _init functions defined by the ZMK_DISPLAY_WIDGET_LISTENER macro.
     * We need to call them, but we don't need to care about them.
     */
    battery_widget_init();

    return 0;
}

lv_obj_t *zmk_screen_widget_obj(struct zmk_screen_widget *widget) { return widget->obj; }