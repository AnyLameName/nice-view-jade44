#include <zephyr/kernel.h>

#include "../include/util.h"
#include "include/connections.h"

void draw_conn_status(lv_obj_t *widget, const struct conn_state *conn) {
    // Canvases don't get names we can reference here, but it was named "top" when it was created.
    lv_obj_t *canvas = lv_obj_get_child(widget, CANVAS_NUM_MID);
    blank_background(canvas);

    // Basic connection status
    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &lv_font_unscii_8, LV_TEXT_ALIGN_LEFT);

    char bt_profile_text[25] = {0};
    snprintf(bt_profile_text, sizeof(bt_profile_text), "BT: %d, %s", conn->active_index, conn->is_connected ? "active" : "dead");

    canvas_draw_text(canvas, 0, 0, CANVAS_SIZE, &label_dsc, bt_profile_text);

    // Always finish by rotating to correct orientation.
    rotate_canvas(canvas);
}

struct conn_state conn_get_state(const zmk_event_t *eh) {
   return (struct conn_state) {
       .active_index = zmk_ble_active_profile_index(),
       .is_bonded = !zmk_ble_active_profile_is_open(),
       .is_connected = zmk_ble_active_profile_is_connected(),
       .selected_endpoint = zmk_endpoint_get_selected(),
   };
}
