#include "init.hpp"

LOG_MODULE_DECLARE(midair);

struct devices dev = {  };

static atomic_t is_initialized = ATOMIC_INIT(false);;

#define DEVICE_GET_BINDING(port, name)   \
    (port) = device_get_binding((name)); \
    if ((port) == NULL) {                                     \
        LOG_ERR("device_get_binding(\"%s\") failed", (name)); \
        k_panic();                                            \
    }

static void configure_gpio_pins(void);

static void verify_pwm_configs(void);
static void verify_counter_configs(void);

void devices_init(void) {

    if (atomic_get(&is_initialized)) return;

    DEVICE_GET_BINDING(dev.entropy, CONFIG_ENTROPY_NAME);

    DEVICE_GET_BINDING(dev.gpio0, DT_NORDIC_NRF_GPIO_0_LABEL);

    DEVICE_GET_BINDING(dev.red_led, DT_GPIO_LEDS_LED0_RED_GPIO_CONTROLLER);
    DEVICE_GET_BINDING(dev.grn_led, DT_GPIO_LEDS_LED1_GREEN_GPIO_CONTROLLER);

    DEVICE_GET_BINDING(dev.pwm0, DT_NORDIC_NRF_PWM_0_LABEL);
    DEVICE_GET_BINDING(dev.pwm1, DT_NORDIC_NRF_PWM_1_LABEL);

    DEVICE_GET_BINDING(dev.red_btn, DT_GPIO_KEYS_BUTTON0_RED_GPIO_CONTROLLER);
    DEVICE_GET_BINDING(dev.grn_btn, DT_GPIO_KEYS_BUTTON1_GREEN_GPIO_CONTROLLER);

    DEVICE_GET_BINDING(dev.saadc0, DT_NORDIC_NRF_SAADC_0_LABEL);

    DEVICE_GET_BINDING(dev.spi0,   DT_NORDIC_NRF_SPI_SPI_0_LABEL);
    DEVICE_GET_BINDING(dev.spi0cs, DT_NORDIC_NRF_SPI_0_CS_GPIOS_CONTROLLER);

    DEVICE_GET_BINDING(dev.i2c1, DT_NORDIC_NRF_I2C_I2C_1_LABEL);

    DEVICE_GET_BINDING(dev.rtc2, DT_NORDIC_NRF_RTC_RTC_2_LABEL);

    DEVICE_GET_BINDING(dev.timer1, DT_NORDIC_NRF_TIMER_TIMER_1_LABEL);
    DEVICE_GET_BINDING(dev.timer2, DT_NORDIC_NRF_TIMER_TIMER_2_LABEL);
    DEVICE_GET_BINDING(dev.timer3, DT_NORDIC_NRF_TIMER_TIMER_3_LABEL);
    DEVICE_GET_BINDING(dev.timer4, DT_NORDIC_NRF_TIMER_TIMER_4_LABEL);

    configure_gpio_pins();

    verify_pwm_configs();
    verify_counter_configs();

    atomic_set(&is_initialized, true);

}


// Santity check the hard-coded pin numbers, below:
//
STATIC_ASSERT(DT_NORDIC_NRF_SPI_0_CS_GPIOS_PIN == 3);
STATIC_ASSERT(DT_NORDIC_NRF_I2C_I2C_1_SCL_PIN ==  9);
STATIC_ASSERT(DT_NORDIC_NRF_I2C_I2C_1_SDA_PIN == 10);
//
static void configure_gpio_pins(void) {

    struct {

        struct device *port;
        u32_t pin;
        int flags;
        u32_t state;

    } const config[] = {

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

    k_sleep(100); // this delay ensures that the SPI and I2C bus states are reset

}


static void verify_pwm_configs(void) {

    struct {

        struct device *port;
        u32_t pin;

    } const config[] = {

        { dev.pwm0, to_underlying(led_pin::red) },
        { dev.pwm1, to_underlying(led_pin::green) },

    };

    for (u32_t i = 0; i < ARRAY_SIZE(config); i++) {

        u64_t cycles;
        int failed = pwm_get_cycles_per_sec(dev.pwm0, to_underlying(led_pin::red), &cycles);

        if (failed) {
            LOG_ERR("pwm_get_cycles_per_sec failed for config[%u]", i);
            k_panic();
        }

        if (cycles != UINT64_C(16000000)) {
            LOG_ERR("unexpected pwm_get_cycles_per_sec: 0x%08x%08x", u32_t(cycles >> 32), u32_t(cycles));
            k_panic();
        }

    }

}


static void verify_counter_configs(void) {

    struct counter {

        struct device *port;
        u32_t frequency;
        bool is_counting_up;
        u8_t alarm_channels;
        u32_t top_value;

    } const config[] = {

        { .port = dev.rtc2,   .frequency = 32768,   .is_counting_up = true, .alarm_channels = 3, .top_value = 0x00FFFFFF },
        { .port = dev.timer1, .frequency = 1000000, .is_counting_up = true, .alarm_channels = 2, .top_value = 0xFFFFFFFF },
        { .port = dev.timer2, .frequency = 1000000, .is_counting_up = true, .alarm_channels = 2, .top_value = 0xFFFFFFFF },
        { .port = dev.timer3, .frequency = 1000000, .is_counting_up = true, .alarm_channels = 4, .top_value = 0xFFFFFFFF },
        { .port = dev.timer4, .frequency = 1000000, .is_counting_up = true, .alarm_channels = 4, .top_value = 0xFFFFFFFF },

    };

    for (u32_t i = 0; i < ARRAY_SIZE(config); i++) {

        struct device *port = config[i].port;

        struct counter instance = {
            .port = port,
            .frequency = counter_get_frequency(port),
            .is_counting_up = counter_is_counting_up(port),
            .alarm_channels = counter_get_num_of_channels(port),
            .top_value = counter_get_top_value(port),
        };

        if (
            (instance.port != config[i].port) ||
            (instance.frequency != config[i].frequency) ||
            (instance.is_counting_up != config[i].is_counting_up) ||
            (instance.alarm_channels != config[i].alarm_channels) ||
            (instance.top_value != config[i].top_value)
        ) {

            LOG_ERR("counter[%u] want: { frequency: %u, is_counting_up: %d, alarm_channels: %d, top_value: 0x%08x }", i, config[i].frequency, config[i].is_counting_up, config[i].alarm_channels, config[i].top_value);
            LOG_ERR("counter[%u] have: { frequency: %u, is_counting_up: %d, alarm_channels: %d, top_value: 0x%08x }", i,  instance.frequency,  instance.is_counting_up,  instance.alarm_channels,  instance.top_value);
            k_panic();

        }

    }

}
