//
// Copyright (c) 2019 Andrew Fernandes <andrew@fernandes.org>
//
// SPDX-License-Identifier: Apache-2.0
//
#include "main.hpp"
#include "init.hpp"

static void configure_buttons(void);
static void button_changed(struct device *dev, struct gpio_callback *cb, u32_t pins);

LOG_MODULE_REGISTER(midair, LOG_LEVEL_DBG);

void main(void) {

    devices_init();

    configure_lsm6dsox();
    configure_mmc5883ma();
    configure_buttons();

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


static void configure_buttons(void) {

	static struct gpio_callback button_cb;

	gpio_pin_configure(dev.red_btn, RED_BTN_PIN, (GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_INT | GPIO_INT_DEBOUNCE | GPIO_INT_EDGE | GPIO_INT_DOUBLE_EDGE));
    gpio_pin_configure(dev.grn_btn, GRN_BTN_PIN, (GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_INT | GPIO_INT_DEBOUNCE | GPIO_INT_EDGE | GPIO_INT_DOUBLE_EDGE));

    gpio_init_callback(&button_cb, button_changed, BIT(RED_BTN_PIN) | BIT(GRN_BTN_PIN));

    gpio_add_callback(dev.red_btn, &button_cb);
    gpio_add_callback(dev.grn_btn, &button_cb);

	gpio_pin_enable_callback(dev.red_btn, RED_BTN_PIN);
	gpio_pin_enable_callback(dev.grn_btn, GRN_BTN_PIN);

}


static void button_changed(struct device *dev_btn, struct gpio_callback *cb, u32_t pins) {

    static const char * const state[2] = { "released", "pressed" };

    if (pins & BIT(RED_BTN_PIN)) {

        u32_t value;
        gpio_pin_read(dev_btn, RED_BTN_PIN, &value);
        value = value ? 0 : 1; // negated

        gpio_pin_write(dev.red_led, RED_LED_PIN, value);
        atomic_set(&red_btn_state, value);

        LOG_INF("red: %s", state[value]);

    }

    if (pins & BIT(GRN_BTN_PIN)) {

        u32_t value;
        gpio_pin_read(dev_btn, GRN_BTN_PIN, &value);
        value = value ? 0 : 1; // negated

        gpio_pin_write(dev.grn_led, GRN_LED_PIN, value);
        atomic_set(&grn_btn_state, value);

        LOG_INF("green: %s", state[value]);

    }

}

