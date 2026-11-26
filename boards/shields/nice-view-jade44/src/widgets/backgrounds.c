#include <lvgl.h>
#include <util.h>

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_MAIN_BG
#define LV_ATTRIBUTE_MAIN_BG
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_MAIN_BG uint8_t main_bg_data [] = {
    0x00, 0x00, 0x00, 0xff, // Color of index 0*/
    0xff, 0xff, 0xff, 0xff, // Color of index 1*/
    0xaa, 0xbb, 0x00, 0x00, // Actual image data, once we have it, replaces this.
};

const lv_img_dsc_t main_background = {
    .header = {
        .cf = LV_COLOR_FORMAT_I1,
        .w = PANEL_WIDTH,
        .h = PANEL_HEIGHT,
    },

    .data = main_bg_data,
    .data_size = sizeof(main_bg_data),
};