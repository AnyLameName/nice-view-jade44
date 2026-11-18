#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/split/bluetooth/peripheral.h>
#include <zmk/events/split_peripheral_status_changed.h>
#include <zmk/usb.h>
#include <zmk/ble.h>

#include "include/second_screen.h"
#include "include/util.h"
#include "widgets/include/battery.h"

static void set_battery_status(struct zmk_screen_widget *widget,
                               struct battery_state new_battery_state) {
    widget->state.battery = new_battery_state;

    draw_battery(widget->obj, &widget->state.battery);
}

static void battery_status_update_cb(struct battery_state state) {
    struct zmk_screen_widget *widget;
    set_battery_status(widget, state);
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);

int zmk_screen_widget_init(struct zmk_screen_widget *widget, lv_obj_t *screen) {
    widget->obj = lv_obj_create(screen);
    lv_obj_set_size(widget->obj, SCREEN_HEIGHT, SCREEN_WIDTH);
    lv_obj_set_style_bg_color(widget->obj, LVGL_BACKGROUND, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t *top = lv_canvas_create(widget->obj);
    lv_obj_align(top, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_canvas_set_buffer(top, widget->cbuf, CANVAS_SIZE, CANVAS_SIZE, CANVAS_COLOR_FORMAT);

    widget_battery_status_init();

    return 0;
}

lv_obj_t *zmk_screen_widget_obj(struct zmk_screen_widget *widget) { return widget->obj; }
