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
    insist(pwm_pin_set_usec(dev.red_pwm, to_underlying(led_pin::red), period, pulse, RED_LED_PWM_FLAGS));

}

void set_grn_led_state(u8_t brightness) {

    const u32_t pulse = (brightness * multiplier);
    insist(pwm_pin_set_usec(dev.grn_pwm, to_underlying(led_pin::grn), period, pulse, GRN_LED_PWM_FLAGS));

}

//----------------------------------------------------------------------
