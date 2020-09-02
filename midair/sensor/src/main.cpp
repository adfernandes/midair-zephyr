#include "main.hpp"
#include "devices.hpp"

#include "leds.hpp"
#include "buttons.hpp"
#include "battery.hpp"

#include "lsm6dsox.hpp"
#include "mmc5883ma.hpp"

LOG_MODULE_REGISTER(midair, LOG_LEVEL_DBG);

//----------------------------------------------------------------------
//
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif // !defined(__clang__)
//
void main(void) {

    devices_init();

    configure_leds();
    configure_buttons();
    configure_lsm6dsox();
    configure_mmc5883ma();
    configure_battery();

    unsigned foo = 1;
    __ASSERT(foo == 0xF0CACC1A, "Invalid value of foo, got 0x%x", foo);

    insist(counter_start(dev.rtc2));

    while (true) {

        const bool red_pushed = get_red_btn_state();
        const bool grn_pushed = get_grn_btn_state();

        uint32_t counter_value;
        insist(counter_get_value(dev.rtc2, &counter_value));
        const float seconds = rtc2_rate * counter_value;
        const float radians = 3.14159265358979323846f * seconds;

        uint8_t red_bright = uint8_t(lroundf(255.0f * (0.5f * (sin(radians) + 1.0f))));
        uint8_t grn_bright = UINT8_MAX - red_bright;

        set_red_led_state(red_bright | (red_pushed ? 0x80 : 0x00));
        set_grn_led_state(grn_bright | (grn_pushed ? 0x80 : 0x00));

        if (!isfinite(seconds)) break; // no 'endless loop' warning

        k_msleep(32);

    }

}
//
#if defined(__clang__)
#pragma clang diagnostic pop
#endif // !defined(__clang__)
//
//----------------------------------------------------------------------
