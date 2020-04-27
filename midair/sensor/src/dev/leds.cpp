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
// TODO
// - Set brightness via 10-level log2 based lookup table
// - use the power API to turn the PWM peripheral off and on when the level is zero
//   https://docs.zephyrproject.org/latest/reference/power_management/index.html?highlight=config_device_power_management#_CPPv422device_set_power_stateP6device5u32_t12device_pm_cbPv
//   (and also see the 'samples' directory 'zephyr/samples/sensor/apds9960/src/main.c' for example)
//   ... we want DEVICE_PM_ACTIVE_STATE and DEVICE_PM_OFF_STATE according to 'pwm_nrfx.c' ...
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
