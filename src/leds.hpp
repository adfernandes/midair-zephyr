#pragma once

//----------------------------------------------------------------------

#include "main.hpp"
#include "devices.hpp"

enum class led_pin : u32_t {
    red   = RED_LED_PIN,
    grn   = GRN_LED_PIN,
};

void configure_leds(void);

//----------------------------------------------------------------------
