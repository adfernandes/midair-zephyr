#pragma once
//----------------------------------------------------------------------

#include "main.hpp"

#define RED_LED_CONTROLLER DT_GPIO_LEDS_LED0_RED_GPIO_CONTROLLER
#define RED_LED_PIN        DT_GPIO_LEDS_LED0_RED_GPIO_PIN

#define GRN_LED_CONTROLLER DT_GPIO_LEDS_LED1_GREEN_GPIO_CONTROLLER
#define GRN_LED_PIN        DT_GPIO_LEDS_LED1_GREEN_GPIO_PIN

enum class led_id { red, green };

// TODO: START HERE

//----------------------------------------------------------------------
