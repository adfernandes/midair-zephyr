#ifndef DEVICES_H
#define DEVICES_H
//----------------------------------------------------------------------

struct devices {

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

#define RED_LED_CONTROLLER DT_GPIO_LEDS_LED0_RED_GPIO_CONTROLLER
#define RED_LED_PIN        DT_GPIO_LEDS_LED0_RED_GPIO_PIN

#define GRN_LED_CONTROLLER DT_GPIO_LEDS_LED1_GREEN_GPIO_CONTROLLER
#define GRN_LED_PIN        DT_GPIO_LEDS_LED1_GREEN_GPIO_PIN

#define RED_BTN_CONTROLLER DT_GPIO_KEYS_BUTTON0_RED_GPIO_CONTROLLER
#define RED_BTN_PIN        DT_GPIO_KEYS_BUTTON0_RED_GPIO_PIN

#define GRN_BTN_CONTROLLER DT_GPIO_KEYS_BUTTON1_GREEN_GPIO_CONTROLLER
#define GRN_BTN_PIN        DT_GPIO_KEYS_BUTTON1_GREEN_GPIO_PIN

//----------------------------------------------------------------------
#endif // DEVICES_H
