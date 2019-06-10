//
// Copyright (c) 2019 Andrew Fernandes <andrew@fernandes.org>
//
// SPDX-License-Identifier: Apache-2.0
//
#include "main.hpp"
#include "devices.hpp"

#include "leds.hpp"
#include "buttons.hpp"

#include "lsm6dsox.hpp"
#include "mmc5883ma.hpp"

LOG_MODULE_REGISTER(midair, LOG_LEVEL_DBG);

void main(void) {

    devices_init();

    configure_leds();
    configure_buttons();
    configure_lsm6dsox();
    configure_mmc5883ma();

    insist(counter_start(dev.rtc2));

    while (true) {

        const bool red_pushed = get_red_btn_state();
        const bool grn_pushed = get_grn_btn_state();

        const float seconds = rtc2_rate * counter_read(dev.rtc2);
        const float radians = 3.14159265358979323846f * seconds;

        u8_t red_bright = u8_t(255.0f * (0.5f * (sin(radians) + 1.0f)) + 0.5f);
        u8_t grn_bright = u8_t(255.0f * (0.5f * (cos(radians) + 1.0f)) + 0.5f);

        set_red_led_state(red_bright | (red_pushed ? 0x80 : 0x00));
        set_grn_led_state(grn_bright | (grn_pushed ? 0x80 : 0x00));

        k_sleep(32); // milliseconds

    }

}

