#pragma once

#include <lvgl.h>
#include <stdint.h>
#include <zmk/event_manager.h>

struct battery_state {
    uint8_t level;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    bool usb_present;
#endif
};

void draw_top(lv_obj_t *widget, const struct battery_state *battery);
struct battery_state battery_status_get_state(const zmk_event_t *eh);
