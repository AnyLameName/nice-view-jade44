#pragma once

#include <lvgl.h>

struct layer_state
{
    uint8_t index;
    const char *label;
};

void draw_layer_status(lv_obj_t *widget, const struct layer_state *layer);
static void set_layer_status(struct zmk_screen_widget *widget, struct layer_state new_layer_state);
static struct layer_state layer_get_state(const zmk_event_t *eh);
static void layer_update_cb(struct layer_state state);
