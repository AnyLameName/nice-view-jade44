#include <zephyr/kernel.h>
#include "util.h"

void rotate_canvas(lv_obj_t *canvas) {
    uint8_t *buf = lv_canvas_get_draw_buf(canvas)->data;
    static uint8_t buf_copy[CANVAS_BUF_SIZE];
    memcpy(buf_copy, buf, sizeof(buf_copy));

    /* CANVAS_SIZE showing up again, used as src_width and src_height. What?
     * 
     * Another potential clue into the weird lv_obj_align offsets, though. Should have noticed this earlier, 
     * but the canvas buffers are squares. The screen is a rectangle, and the widgets are (almost?) all rectangles.
     * I feel like this is being solved in a very weird way.
     * 
     * Not yet sure if it's being done poorly or if the most efficient way just looks really weird.
     */
    const uint32_t stride = lv_draw_buf_width_to_stride(CANVAS_SIZE, CANVAS_COLOR_FORMAT);
    lv_draw_sw_rotate(buf_copy, buf, CANVAS_SIZE, CANVAS_SIZE, stride, stride,
                      LV_DISPLAY_ROTATION_270, CANVAS_COLOR_FORMAT);
}

void fill_background(lv_obj_t *canvas) {
    lv_canvas_fill_bg(canvas, LVGL_BACKGROUND, LV_OPA_COVER);
}

void init_label_dsc(lv_draw_label_dsc_t *label_dsc, lv_color_t color, const lv_font_t *font,
                    lv_text_align_t align) {
    lv_draw_label_dsc_init(label_dsc);
    label_dsc->color = color;
    label_dsc->font = font;
    label_dsc->align = align;
}

void canvas_draw_text(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, lv_coord_t max_w,
                      lv_draw_label_dsc_t *draw_dsc, const char *txt) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    draw_dsc->text = txt;
    /*
     * Why are we adding CANVAS_SIZE aka 68 to the y value?
     * And does the fact that we use the same CANVAS_SIZE as the height of the canvas
     * have anything to do with the fact that the bottom canvas is offset by -44?
     * 
     * Could we do all of that much cleaner if we just use proper canvas sizes?
     */
    lv_area_t coords = {x, y, x + max_w, y + CANVAS_SIZE};
    lv_draw_label(&layer, draw_dsc, &coords);

    lv_canvas_finish_layer(canvas, &layer);
}