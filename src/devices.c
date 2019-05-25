#include "main.h"

LOG_MODULE_DECLARE(midair);

struct devices dev = {  };

static atomic_t is_initialized = ATOMIC_INIT(false);;

#define DEVICE_GET_BINDING(var, label)   \
    (var) = device_get_binding((label)); \
    if ((var) == NULL) {                                       \
        LOG_ERR("device_get_binding(\"%s\") failed", (label)); \
        k_panic();                                             \
    }

static void configure_gpio_pins(void);

void devices_init(void) {

    if (atomic_get(&is_initialized)) return;

    DEVICE_GET_BINDING(dev.gpio0, DT_NORDIC_NRF_GPIO_0_LABEL);

    DEVICE_GET_BINDING(dev.red_led, RED_LED_CONTROLLER);
    DEVICE_GET_BINDING(dev.grn_led, GRN_LED_CONTROLLER);

    DEVICE_GET_BINDING(dev.red_btn, RED_BTN_CONTROLLER);
    DEVICE_GET_BINDING(dev.grn_btn, GRN_BTN_CONTROLLER);

    DEVICE_GET_BINDING(dev.saadc0, DT_NORDIC_NRF_SAADC_0_LABEL);

    DEVICE_GET_BINDING(dev.spi0, DT_SPI_0_NAME);
    DEVICE_GET_BINDING(dev.spi0cs, DT_NORDIC_NRF_SPI_0_CS_GPIOS_CONTROLLER);

    DEVICE_GET_BINDING(dev.i2c1, DT_I2C_1_NAME);

    DEVICE_GET_BINDING(dev.rtc2, DT_NORDIC_NRF_RTC_RTC_2_LABEL);

    DEVICE_GET_BINDING(dev.timer1, DT_NORDIC_NRF_TIMER_TIMER_1_LABEL);
    DEVICE_GET_BINDING(dev.timer2, DT_NORDIC_NRF_TIMER_TIMER_2_LABEL);
    DEVICE_GET_BINDING(dev.timer3, DT_NORDIC_NRF_TIMER_TIMER_3_LABEL);
    DEVICE_GET_BINDING(dev.timer4, DT_NORDIC_NRF_TIMER_TIMER_4_LABEL);

    configure_gpio_pins();

    atomic_set(&is_initialized, true);

}


static void configure_gpio_pins(void) {

    struct {

        struct device *port;
        u32_t pin;
        int flags;
        u32_t state;

    } config[] = {

        // LED Output Pins ---------------------------------------------

        { dev.red_led, RED_LED_PIN, (GPIO_DIR_OUT | GPIO_PUD_NORMAL), 0 },
        { dev.grn_led, GRN_LED_PIN, (GPIO_DIR_OUT | GPIO_PUD_NORMAL), 0 },

        // Button Input Pins -------------------------------------------

	    { dev.red_btn, RED_BTN_PIN, (GPIO_DIR_IN | GPIO_PUD_PULL_UP) },
        { dev.grn_btn, GRN_BTN_PIN, (GPIO_DIR_IN | GPIO_PUD_PULL_UP) },

        // SSADC Pre-Configuration -------------------------------------

        { dev.gpio0,  4, (GPIO_DIR_IN | GPIO_PUD_NORMAL) },

        // LSM6DSOX Accelerometer --------------------------------------

        // Note that INT1 must be connected to logical zero or left floating
        // during power-on if the I2C or SPI busses are to be used or enabled.
        //
        // LSM6DSOX power-on states:
        //  - CS:   input with pull-up
        //  - INT1: input with pull-down
        //  - INT2: output forced to ground
        //
        // When set as outputs, the INT1 and INT2 lines are active-high.

        { dev.gpio0,  3, (GPIO_DIR_OUT | GPIO_PUD_NORMAL), 1 }, // spi0.cs
        { dev.gpio0, 22, (GPIO_DIR_IN  | GPIO_PUD_NORMAL) },    // spi0.int1
        { dev.gpio0,  2, (GPIO_DIR_IN  | GPIO_PUD_NORMAL) },    // spi0.int2

        // MMC5883MA Magnetometer --------------------------------------

        // Upon power-on the INT line is held as high-z until the interrupt
        // functions are enabled, whereupon it is actively driven high and
        // low, and is active-high.

        // Note that rather than using discrete components for additional
        // I2C pull-up resistors, the pull-ups of ganged GPIO lones are used
        // to set the resistance in parallel. The nRF52 series has pull-up
        // and pull-down resistors of 11k/13k/16k min/nominal/max value,
        // so the effective pull-up is one to one-third this value,
        // depending on how many lines are used.

        { dev.gpio0,  9, (GPIO_DIR_IN | GPIO_PUD_PULL_UP) },    // i2c1.scl
        { dev.gpio0, 10, (GPIO_DIR_IN | GPIO_PUD_PULL_UP) },    // i2c1.sda
        { dev.gpio0, 12, (GPIO_DIR_IN | GPIO_PUD_PULL_UP) },    // i2c1.scl.pullup
        { dev.gpio0, 19, (GPIO_DIR_IN | GPIO_PUD_PULL_UP) },    // i2c1.scl.pullup
        { dev.gpio0, 11, (GPIO_DIR_IN | GPIO_PUD_PULL_UP) },    // i2c1.sda.pullup
        { dev.gpio0, 14, (GPIO_DIR_IN | GPIO_PUD_PULL_UP) },    // i2c1.sda.pullup
        { dev.gpio0, 26, (GPIO_DIR_IN | GPIO_PUD_NORMAL)  },    // i2c1.int

        // -------------------------------------------------------------

    };

    for (u32_t i = 0; i < ARRAY_SIZE(config); i++) {

        int err;

        err = gpio_pin_configure(config[i].port, config[i].pin, config[i].flags);
        if (err != 0) {
            LOG_ERR("failed config[%u] pin %u with errno %d", i, config[i].pin, err);
            k_panic();
        }

        if (config[i].flags & GPIO_DIR_OUT) {

            err = gpio_pin_write(config[i].port, config[i].pin, config[i].state);
            if (err != 0) {
                LOG_ERR("failed config[%u] pin %u with errno %d", i, config[i].pin, err);
                k_panic();
            }

        }

    }

    k_sleep(100); // milliseconds // this delay resets the SPI and I2C bus states

}
