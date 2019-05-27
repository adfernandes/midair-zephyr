#pragma once
//----------------------------------------------------------------------

#include "main.hpp"

#include "leds.hpp"
#include "btns.hpp"

struct devices {

    struct device *entropy;

    struct device *gpio0;

    struct device *red_led;
    struct device *grn_led;

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
