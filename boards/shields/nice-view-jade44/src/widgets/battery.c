#include <zephyr/kernel.h>

#include <zmk/battery.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/usb.h>

#include <util.h>
#include <battery.h>

void draw_battery(lv_obj_t *widget, const struct battery_state *battery)
{
    lv_obj_t *canvas = lv_obj_get_child(widget, CANVAS_NUM_TOP);
    blank_background(canvas);

    // Basic battery number
    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &lv_font_unscii_8, LV_TEXT_ALIGN_LEFT);

    char battery_text[10] = {0};
    snprintf(battery_text, sizeof(battery_text), "Bat: %d%%", battery->level);

    canvas_draw_text(canvas, 0, 0, CANVAS_SIZE, &label_dsc, battery_text);

    // Always finish by rotating to correct orientation.
    rotate_canvas(canvas);
}

struct battery_state battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

    return (struct battery_state){
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .usb_present = zmk_usb_is_powered(),
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */
    };
}
