#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>
#include "util.h"

struct zmk_screen_widget {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_color_t cbuf[CANVAS_SIZE * CANVAS_SIZE];
    struct status_state state;
};

int zmk_screen_widget_init(struct zmk_screen_widget *widget, lv_obj_t *parent);
lv_obj_t *zmk_screen_widget_obj(struct zmk_screen_widget *widget);