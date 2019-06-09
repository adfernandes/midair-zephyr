#include "buttons.hpp"

LOG_MODULE_DECLARE(midair, LOG_LEVEL_DBG);

//----------------------------------------------------------------------

atomic_t red_btn_state = ATOMIC_INIT(0);
atomic_t grn_btn_state = ATOMIC_INIT(0);

//----------------------------------------------------------------------

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

//----------------------------------------------------------------------

void configure_buttons(void) {

	static struct gpio_callback button_cb;

	gpio_pin_configure(dev.red_btn, RED_BTN_PIN, (GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_INT | GPIO_INT_DEBOUNCE | GPIO_INT_EDGE | GPIO_INT_DOUBLE_EDGE));
    gpio_pin_configure(dev.grn_btn, GRN_BTN_PIN, (GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_INT | GPIO_INT_DEBOUNCE | GPIO_INT_EDGE | GPIO_INT_DOUBLE_EDGE));

    gpio_init_callback(&button_cb, button_changed, BIT(RED_BTN_PIN) | BIT(GRN_BTN_PIN));

    gpio_add_callback(dev.red_btn, &button_cb);
    gpio_add_callback(dev.grn_btn, &button_cb);

	gpio_pin_enable_callback(dev.red_btn, RED_BTN_PIN);
	gpio_pin_enable_callback(dev.grn_btn, GRN_BTN_PIN);

}

//----------------------------------------------------------------------
