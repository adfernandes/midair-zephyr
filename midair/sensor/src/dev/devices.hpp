#pragma once

//----------------------------------------------------------------------

#include "main.hpp"

struct devices {

    const struct device *clock;

    const struct device *entropy;

    const struct device *gpio0;

    const struct device *red_led;
    const struct device *grn_led;

    const struct device *red_pwm;
    const struct device *grn_pwm;

    const struct device *red_btn;
    const struct device *grn_btn;

    const struct device *spi0;
    const struct device *spi0cs;

    const struct device *i2c1;

    // RTC0 is Reserved;
    // RTC1 is Reserved;
    const struct device *rtc2;

    // TIMER0 is Reserved;
    const struct device *timer1;
    const struct device *timer2;
    const struct device *timer3;
    const struct device *timer4;

};

extern struct devices dev;

void devices_init(void);

//----------------------------------------------------------------------

const uint32_t rtc2_frequency = 32768; // Hz
const float rtc2_rate = 1.0f / float(rtc2_frequency);

const uint32_t timer1_frequency = 1000000; // Hz
const uint32_t timer2_frequency = 1000000; // Hz
const uint32_t timer3_frequency = 1000000; // Hz
const uint32_t timer4_frequency = 1000000; // Hz

//----------------------------------------------------------------------

#define RED_LED_PIN DT_ALIAS_LED0_RED_GPIOS_PIN
#define GRN_LED_PIN DT_ALIAS_LED1_GREEN_GPIOS_PIN

#define RED_BTN_PIN DT_ALIAS_BUTTON0_RED_GPIOS_PIN
#define GRN_BTN_PIN DT_ALIAS_BUTTON1_GREEN_GPIOS_PIN

#define RED_LED_PWM_FLAGS PWM_POLARITY_NORMAL
#define GRN_LED_PWM_FLAGS PWM_POLARITY_NORMAL

//----------------------------------------------------------------------
