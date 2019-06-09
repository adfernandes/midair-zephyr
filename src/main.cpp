//
// Copyright (c) 2019 Andrew Fernandes <andrew@fernandes.org>
//
// SPDX-License-Identifier: Apache-2.0
//
#include "main.hpp"
#include "devices.hpp"

LOG_MODULE_REGISTER(midair, LOG_LEVEL_DBG);

void main(void) {

    devices_init();

    configure_buttons();

    configure_lsm6dsox();
    configure_mmc5883ma();

    u32_t count = 0;
    counter_start(dev.rtc2);

    while (true) {

        const bool current_red_btn_state = get_red_btn_state();
        const bool current_grn_btn_state = get_grn_btn_state();

        LOG_DBG("count: %u, rtc: 0x%08x, red: %u, green: %u",
            count, counter_read(dev.rtc2), current_red_btn_state, current_grn_btn_state);

        gpio_pin_write(dev.red_led, RED_LED_PIN,  (count & 1) || current_red_btn_state);
        gpio_pin_write(dev.grn_led, GRN_LED_PIN, !(count & 1) || current_grn_btn_state);

        k_sleep(1000); // milliseconds
        count++;

    }

}

