#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>
#include "include/util.h"

struct zmk_screen_widget
{
    lv_obj_t *obj;
    uint8_t top_buffer[CANVAS_BUF_SIZE];
    uint8_t mid_buffer[CANVAS_BUF_SIZE];
    uint8_t low_buffer[CANVAS_BUF_SIZE];
    struct widget_state state;
};

int zmk_screen_widget_init(struct zmk_screen_widget *widget, lv_obj_t *parent);
lv_obj_t *zmk_screen_widget_obj(struct zmk_screen_widget *widget);