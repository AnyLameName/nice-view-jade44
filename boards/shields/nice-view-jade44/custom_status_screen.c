#include "widgets/main_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_NICE_VIEW_WIDGET_STATUS)
static struct zmk_screen_widget screen_widget;
#endif

/* This hook will be called by zmk.
 * It overrides a definition in zmk's app/src/display/main.c and is called by initialize_display(...) in that file.
 */
lv_obj_t *zmk_display_status_screen() {

    lv_obj_t *screen;
    screen = lv_obj_create(NULL); // Passing NULL here means no parent object, making it a root screen object.
    
    LOG_WRN("Creating Jade44 status screen");

#if IS_ENABLED(CONFIG_NICE_VIEW_WIDGET_STATUS)
    zmk_screen_widget_init(&screen_widget, screen);
    lv_obj_align(zmk_screen_widget_obj(&screen_widget), LV_ALIGN_TOP_LEFT, 0, 0);
#endif

    return screen;
}
