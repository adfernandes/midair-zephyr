#pragma once

//----------------------------------------------------------------------

#include "main.hpp"
#include "devices.hpp"

enum class led_pin : u32_t {
    red   = RED_LED_PIN,
    grn   = GRN_LED_PIN,
};

void configure_leds(void);

void set_red_led_state(u8_t brightness);
void set_grn_led_state(u8_t brightness);

//----------------------------------------------------------------------
