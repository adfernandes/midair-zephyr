#include "buttons.hpp"

LOG_MODULE_DECLARE(midair, LOG_LEVEL_DBG);

//----------------------------------------------------------------------

atomic_t red_btn_state = ATOMIC_INIT(0);
atomic_t grn_btn_state = ATOMIC_INIT(0);

static struct gpio_callback button_cb;
static const char * const state[2] = { "released", "pressed" };

//----------------------------------------------------------------------

static void button_changed(struct device *dev_btn, struct gpio_callback *cb, u32_t pins) {

    if (pins & BIT(RED_BTN_PIN)) {

        const u32_t value = gpio_pin_get(dev_btn, RED_BTN_PIN);
        insist(gpio_pin_set(dev.red_led, RED_LED_PIN, value));
        atomic_set(&red_btn_state, value);

        LOG_INF("red: %s", state[value]);

    }

    if (pins & BIT(GRN_BTN_PIN)) {

        const u32_t value = gpio_pin_get(dev_btn, GRN_BTN_PIN);
        insist(gpio_pin_set(dev.grn_led, GRN_LED_PIN, value));
        atomic_set(&grn_btn_state, value);

        LOG_INF("green: %s", state[value]);

    }

}

//----------------------------------------------------------------------

void configure_buttons(void) {

    // the pins have already been configured via 'gpio_pin_configure' before this point

    insist(gpio_pin_interrupt_configure(dev.red_btn, RED_BTN_PIN, (GPIO_INT_ENABLE | GPIO_INT_DEBOUNCE | GPIO_INT_EDGE | GPIO_INT_EDGE_BOTH)));
    insist(gpio_pin_interrupt_configure(dev.grn_btn, GRN_BTN_PIN, (GPIO_INT_ENABLE | GPIO_INT_DEBOUNCE | GPIO_INT_EDGE | GPIO_INT_EDGE_BOTH)));

    gpio_init_callback(&button_cb, button_changed, BIT(RED_BTN_PIN) | BIT(GRN_BTN_PIN));

    insist(gpio_add_callback(dev.red_btn, &button_cb));
    insist(gpio_add_callback(dev.grn_btn, &button_cb));

    LOG_DBG("all buttons successfully configured");

}

//----------------------------------------------------------------------
