#pragma once
//----------------------------------------------------------------------

#include "main.hpp"

#define RED_LED_PIN DT_GPIO_LEDS_LED0_RED_GPIO_PIN
#define GRN_LED_PIN DT_GPIO_LEDS_LED1_GREEN_GPIO_PIN

enum class led_pin : u32_t {
    red = RED_LED_PIN,
    green = GRN_LED_PIN,
};

// TODO: START HERE

//----------------------------------------------------------------------
