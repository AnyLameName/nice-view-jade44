#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>

struct conn_state {
    int    active_index;
    bool   is_bonded;
    bool   is_connected;
    struct zmk_endpoint_instance selected_endpoint;
};

void draw_conn_status(lv_obj_t *widget, const struct conn_state *connections);
struct conn_state conn_get_state(const zmk_event_t *eh);
