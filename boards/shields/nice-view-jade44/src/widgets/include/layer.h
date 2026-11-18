#pragma once

#include <lvgl.h>
#include <zmk/event_manager.h>

struct layer_state
{
    uint8_t index;
    const char *label;
};

void draw_layer_status(lv_obj_t *widget, const struct layer_state *layer);
struct layer_state layer_get_state(const zmk_event_t *eh);
