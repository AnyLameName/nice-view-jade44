#pragma once

#include <lvgl.h>
#include <zmk/endpoints.h>

#include "widgets/battery.h"

#define SCREEN_WIDTH 68
#define SCREEN_HEIGHT 160

// These may seem weird, but they exist just to help us remember that the screen is rotated 90 degrees
#define PANEL_WIDTH SCREEN_HEIGHT
#define PANEL_HEIGHT SCREEN_WIDTH

#define CANVAS_SIZE 68
#define CANVAS_COLOR_FORMAT LV_COLOR_FORMAT_L8
#define CANVAS_BUF_SIZE(width, height) \
    LV_CANVAS_BUF_SIZE(width, height, LV_COLOR_FORMAT_GET_BPP(CANVAS_COLOR_FORMAT), LV_DRAW_BUF_STRIDE_ALIGN)

#define LVGL_BACKGROUND                                                                            \
    IS_ENABLED(CONFIG_NICE_VIEW_WIDGET_INVERTED) ? lv_color_black() : lv_color_white()
#define LVGL_FOREGROUND                                                                            \
    IS_ENABLED(CONFIG_NICE_VIEW_WIDGET_INVERTED) ? lv_color_white() : lv_color_black()

struct widget_state {
    struct battery_state battery;
#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
#else
    bool connected;
#endif
};

void rotate_canvas(lv_obj_t *canvas);
void blank_background(lv_obj_t *canvas);
void canvas_draw_text(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, lv_coord_t max_w,
                      lv_draw_label_dsc_t *draw_dsc, const char *txt);
void init_label_dsc(lv_draw_label_dsc_t *label_dsc, lv_color_t color, const lv_font_t *font,
                    lv_text_align_t align);
void init_rect_dsc(lv_draw_rect_dsc_t *rect_dsc, lv_color_t bg_color);