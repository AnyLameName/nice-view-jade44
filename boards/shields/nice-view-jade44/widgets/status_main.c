#include <stdio.h>
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

#include "status_main.h"
#include "util.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static void draw_top(lv_obj_t *widget, const struct status_state *state)
{
    // Canvases don't get names we can reference here, but it was named "top" when it was created.
    lv_obj_t *canvas = lv_obj_get_child(widget, 0);
    blank_background(canvas);
    
    // Basic battery number
    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &lv_font_unscii_8, LV_TEXT_ALIGN_LEFT);
    
    char batteryText[10] = {0};
    snprintf(batteryText, sizeof(batteryText), "Bat: %d%%", state->battery);
    
    canvas_draw_text(canvas, 0, 0, CANVAS_SIZE, &label_dsc, batteryText);
    
    // Always finish by rotating to correct orientation.
    rotate_canvas(canvas);
}

static void set_battery_status(struct zmk_widget_status *widget,
                               struct battery_state state) {
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    widget->state.charging = state.usb_present;
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

    widget->state.battery = state.level;

    draw_top(widget->obj, &widget->state);
}

static void battery_status_update_cb(struct battery_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_status(widget, state); }
}

static struct battery_state battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

    return (struct battery_state){
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .usb_present = zmk_usb_is_powered(),
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */
    };
}

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
ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);

void draw_layer_status(lv_obj_t *widget, const struct layer_state *state) {
    lv_obj_t *canvas = lv_obj_get_child(widget, 2);

    lv_draw_rect_dsc_t rect_black_dsc;
    init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);
    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &lv_font_montserrat_14, LV_TEXT_ALIGN_CENTER);

    // Fill background
    blank_background(canvas);

    // Draw layer
    if (state->layer_label == NULL || strlen(state->layer_label) == 0) {
        char text[10] = {};

        sprintf(text, "LAYER %i", state->layer_index);

        canvas_draw_text(canvas, 0, 5, 68, &label_dsc, text);
    } else {
        canvas_draw_text(canvas, 0, 5, 68, &label_dsc, state->layer_label);
    }

    // Rotate canvas
    rotate_canvas(canvas);
}

static void set_layer_status(struct zmk_widget_status *widget, struct layer_state state) {
    widget->state.layer_index = state.index;
    widget->state.layer_label = state.label;

    draw_layer_status(widget->obj, &widget->state);
}

static void layer_update_cb(struct layer_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_layer_status(widget, state); }
}

static struct layer_state layer_get_state(const zmk_event_t *eh) {
    zmk_keymap_layer_index_t index = zmk_keymap_highest_layer_active();
    return (struct layer_status_state){
        .index = index, .label = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index))};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_state,
                            layer_update_cb, layer_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);

int zmk_widget_status_init(struct zmk_widget_status *widget, lv_obj_t *screen) {
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

    // What's up with this list? What uses it?
    sys_slist_append(&widgets, &widget->node);
    
    /* These _init functions defined by the ZMK_DISPLAY_WIDGET_LISTENER macro.
     * We need to call them but we don't need to care about them.
     */
    widget_battery_status_init();
    widget_layer_status_init();

    return 0;
}

lv_obj_t *zmk_widget_status_obj(struct zmk_widget_status *widget) { return widget->obj; }