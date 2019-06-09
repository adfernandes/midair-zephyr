#include "leds.hpp"

LOG_MODULE_DECLARE(midair, LOG_LEVEL_DBG);

//----------------------------------------------------------------------

static struct {

        struct device *port;
        u32_t pin;

    } const red_led = { dev.pwm0, to_underlying(led_pin::red) },
            grn_led = { dev.pwm1, to_underlying(led_pin::grn) };

const  u32_t       period = 1024 * 1024;    // microseconds
static atomic_t red_pulse = ATOMIC_INIT(0); // microseconds
static atomic_t grn_pulse = ATOMIC_INIT(0); // microseconds

//----------------------------------------------------------------------

void configure_leds(void) {

    int failed = true;

    u32_t current_red_pulse = atomic_get(&red_pulse);
LOG_DBG("====> FOO");
    failed = pwm_pin_set_usec(red_led.port, red_led.pin, period, current_red_pulse);
LOG_DBG("====> BAR %d", failed);
    if (unlikely(failed)) {
        LOG_ERR("failed pwm_pin_set_usec(red_led.port, red_led.pin, %u, %u)", period, current_red_pulse);
        k_panic();
    }

    u32_t current_grn_pulse = atomic_get(&grn_pulse);
    failed = pwm_pin_set_usec(grn_led.port, grn_led.pin, 0, 1024*1024);
    if (unlikely(failed)) {
        LOG_ERR("failed pwm_pin_set_usec(grn_led.port, grn_led.pin, %u, %u)", period, current_grn_pulse);
        k_panic();
    }

}

//----------------------------------------------------------------------
