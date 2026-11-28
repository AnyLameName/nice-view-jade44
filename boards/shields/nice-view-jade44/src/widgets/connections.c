#include <zephyr/kernel.h>

#include <util.h>
#include <connections.h>

LV_IMAGE_DECLARE(frown);
LV_IMAGE_DECLARE(smile);

void draw_conn_status(lv_obj_t *canvas, const struct conn_state *state) {

}

struct conn_state conn_get_state(const zmk_event_t *event) {
    return (struct conn_state) {
        .active_index = zmk_ble_active_profile_index(),
        .is_bonded = !zmk_ble_active_profile_is_open(),
        .is_connected = zmk_ble_active_profile_is_connected(),
        .selected_endpoint = zmk_endpoint_get_selected(),
    };
}
