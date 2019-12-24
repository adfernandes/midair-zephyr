#pragma once

//----------------------------------------------------------------------

#include "main.hpp"

struct devices {

    struct device *clock;

    struct device *entropy;

    struct device *gpio0;

    struct device *red_led;
    struct device *grn_led;

    struct device *red_pwm;
    struct device *grn_pwm;

    struct device *red_btn;
    struct device *grn_btn;

    struct device *saadc0;

    struct device *spi0;
    struct device *spi0cs;

    struct device *i2c1;

    // RTC0 is Reserved;
    // RTC1 is Reserved;
    struct device *rtc2;

    // TIMER0 is Reserved;
    struct device *timer1;
    struct device *timer2;
    struct device *timer3;
    struct device *timer4;

};

extern struct devices dev;

void devices_init(void);

//----------------------------------------------------------------------

const u32_t rtc2_frequency = 32768; // Hz
const float rtc2_rate = 1.0f / float(rtc2_frequency);

const u32_t timer1_frequency = 1000000; // Hz
const u32_t timer2_frequency = 1000000; // Hz
const u32_t timer3_frequency = 1000000; // Hz
const u32_t timer4_frequency = 1000000; // Hz

//----------------------------------------------------------------------

#define RED_LED_PIN DT_ALIAS_LED0_RED_GPIOS_PIN
#define GRN_LED_PIN DT_ALIAS_LED1_GREEN_GPIOS_PIN

#define RED_BTN_PIN DT_ALIAS_BUTTON0_RED_GPIOS_PIN
#define GRN_BTN_PIN DT_ALIAS_BUTTON1_GREEN_GPIOS_PIN

#define BATTERY_ADC_INPUT NRF_SAADC_INPUT_AIN2

//----------------------------------------------------------------------
