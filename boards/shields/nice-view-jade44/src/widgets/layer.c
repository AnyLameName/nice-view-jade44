#include <stdio.h>
#include <string.h>

#include <zmk/keymap.h>

#include <layer.h>
#include <util.h>

void draw_layer_status(lv_obj_t *widget, const struct layer_state *layer) {
    lv_obj_t *canvas = lv_obj_get_child(widget, CANVAS_NUM_BOT);

    lv_draw_rect_dsc_t rect_black_dsc;
    init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);
    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &lv_font_unscii_8, LV_TEXT_ALIGN_CENTER);

    // Fill background
    blank_background(canvas);

    // Draw layer
    if (layer->label == NULL || strlen(layer->label) == 0) {
        char text[10] = {};

        sprintf(text, "LAYER %i", layer->index);

        canvas_draw_text(canvas, 0, 5, 68, &label_dsc, text);
    } else {
        canvas_draw_text(canvas, 0, 5, 68, &label_dsc, layer->label);
    }

    // Rotate canvas
    rotate_canvas(canvas);
}

struct layer_state layer_get_state(const zmk_event_t *eh) {
    zmk_keymap_layer_index_t index = zmk_keymap_highest_layer_active();

    return (struct layer_state){
        .index = index,
        .label = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index))
    };
}
