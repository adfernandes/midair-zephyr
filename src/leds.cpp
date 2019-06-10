#include "leds.hpp"

LOG_MODULE_DECLARE(midair, LOG_LEVEL_DBG);

//----------------------------------------------------------------------

static const u32_t multiplier = 32;
static const u32_t period = (0xFF * multiplier); // microseconds

//----------------------------------------------------------------------

void configure_leds(void) {

    set_red_led_state(0x00);
    set_grn_led_state(0x00);

    LOG_DBG("all leds successfully configured");

}

//----------------------------------------------------------------------

void set_red_led_state(u8_t brightness) {

    const u32_t pulse = (brightness * multiplier);
    int failed = pwm_pin_set_usec(dev.red_pwm, to_underlying(led_pin::red), period, pulse);
    if (unlikely(failed)) {
        LOG_ERR("failed pwm_pin_set_usec(red_led.port, red_led.pin, %u, %u)", period, pulse);
        sys_panic();
    }

}

void set_grn_led_state(u8_t brightness) {

    const u32_t pulse = (brightness * multiplier);
    int failed = pwm_pin_set_usec(dev.grn_pwm, to_underlying(led_pin::grn), period, pulse);
    if (unlikely(failed)) {
        LOG_ERR("failed pwm_pin_set_usec(grn_led.port, grn_led.pin, %u, %u)", period, pulse);
        sys_panic();
    }

}

//----------------------------------------------------------------------
