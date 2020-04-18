#include "devices.hpp"

#include <drivers/clock_control.h>
#include <drivers/clock_control/nrf_clock_control.h>
#include <hal/nrf_power.h>

LOG_MODULE_DECLARE(midair, LOG_LEVEL_DBG);

//----------------------------------------------------------------------

struct devices dev = {  };

static atomic_t is_initialized = ATOMIC_INIT(false);;

#define DEVICE_GET_BINDING(port, name)   \
    (port) = device_get_binding((name)); \
    if (unlikely((port) == NULL)) {                             \
        sys_panic("device_get_binding(\"%s\") failed", (name)); \
    }

//----------------------------------------------------------------------

static void configure_clocks(void);

static void configure_gpio_pins(void);

static void verify_pwm_configs(void);

static void verify_counter_configs(void);

//----------------------------------------------------------------------

void devices_init(void) {

    if (atomic_get(&is_initialized)) return;

    DEVICE_GET_BINDING(dev.clock, DT_INST_0_NORDIC_NRF_CLOCK_LABEL);

    DEVICE_GET_BINDING(dev.entropy, DT_CHOSEN_ZEPHYR_ENTROPY_LABEL);

    DEVICE_GET_BINDING(dev.gpio0, DT_NORDIC_NRF_GPIO_GPIO_0_LABEL);

    DEVICE_GET_BINDING(dev.red_led, DT_ALIAS_LED0_RED_GPIOS_CONTROLLER);
    DEVICE_GET_BINDING(dev.grn_led, DT_ALIAS_LED1_GREEN_GPIOS_CONTROLLER);

    DEVICE_GET_BINDING(dev.red_pwm, DT_PWM_LEDS_PWM_LED_0_PWMS_CONTROLLER);
    DEVICE_GET_BINDING(dev.grn_pwm, DT_PWM_LEDS_PWM_LED_1_PWMS_CONTROLLER);

    DEVICE_GET_BINDING(dev.red_btn, DT_ALIAS_BUTTON0_RED_GPIOS_CONTROLLER);
    DEVICE_GET_BINDING(dev.grn_btn, DT_ALIAS_BUTTON1_GREEN_GPIOS_CONTROLLER);

    DEVICE_GET_BINDING(dev.saadc0, DT_NORDIC_NRF_SAADC_ADC_0_LABEL);

    DEVICE_GET_BINDING(dev.spi0,   DT_NORDIC_NRF_SPIM_SPI_0_LABEL);
    DEVICE_GET_BINDING(dev.spi0cs, DT_NORDIC_NRF_SPIM_SPI_0_CS_GPIOS_CONTROLLER);

    DEVICE_GET_BINDING(dev.i2c1, DT_NORDIC_NRF_TWIM_I2C_1_LABEL);

    DEVICE_GET_BINDING(dev.rtc2, DT_NORDIC_NRF_RTC_RTC_2_LABEL);

    DEVICE_GET_BINDING(dev.timer1, DT_NORDIC_NRF_TIMER_TIMER_1_LABEL);
    DEVICE_GET_BINDING(dev.timer2, DT_NORDIC_NRF_TIMER_TIMER_2_LABEL);
    DEVICE_GET_BINDING(dev.timer3, DT_NORDIC_NRF_TIMER_TIMER_3_LABEL);
    DEVICE_GET_BINDING(dev.timer4, DT_NORDIC_NRF_TIMER_TIMER_4_LABEL);

    configure_clocks();

    configure_gpio_pins();

    verify_pwm_configs();

    verify_counter_configs();

    atomic_set(&is_initialized, true);

}

//----------------------------------------------------------------------

static void configure_clocks(void) {

    // Start both the high-accuracy HF Clock and the LF Clock oscillators, telling the kernel not
    // to change the clocking mode since we are going to use both clocks in "high performance" mode
    //
    // See "zephyr/.../nrf52_clock.{h,c}" for an example on how to use the 'clock_control_*'
    // API function calls as well as online information such as https://tinyurl.com/u8twby7
    //
    // Changes post 2.1 were made as per https://tinyurl.com/yx4hutpc

    clock_control_on(dev.clock, CLOCK_CONTROL_NRF_SUBSYS_HF);
    clock_control_on(dev.clock, CLOCK_CONTROL_NRF_SUBSYS_LF);

    // Disable automatic sleep state transitions, staying always in the "active" state

    sys_pm_force_power_state(SYS_POWER_STATE_ACTIVE);

    // Now wait until the HFXO and LFXO have both started
    //
    // According to the nRF52832 data sheet:
    //  - the typical HFXO startup time is 360 us
    //  - the typical LFXO startup time is 600 us

    k_busy_wait(1285);

    // Verify that everything is running as we expected

    insist_true(nrf_clock_lf_is_running(NRF_CLOCK));
    insist_true(nrf_clock_hf_is_running(NRF_CLOCK, NRF_CLOCK_HFCLK_HIGH_ACCURACY));

    const nrf_clock_lfclk_t nrf_clock_lfclk = nrf_clock_lf_src_get(NRF_CLOCK);
    insist_true(nrf_clock_lfclk == NRF_CLOCK_LFCLK_Xtal || nrf_clock_lfclk == NRF_CLOCK_LFCLK_Synth);

    // Setup the high-performance "constant latency" mode (which is probably moot since we are
    // disabling any sort of power-down or sleep mode that turns off the HFXO or LFXO clocks)

    nrf_power_task_trigger(NRF_POWER, NRF_POWER_TASK_CONSTLAT);

    // Done

    LOG_DBG("configured for maximum performance");

}

//----------------------------------------------------------------------

// Santity check the hard-coded pin numbers, below:
//
STATIC_ASSERT(DT_NORDIC_NRF_SPIM_SPI_0_CS_GPIOS_PIN ==  3);
STATIC_ASSERT(DT_NORDIC_NRF_TWIM_I2C_1_SCL_PIN      ==  9);
STATIC_ASSERT(DT_NORDIC_NRF_TWIM_I2C_1_SDA_PIN      == 10);
//
static void configure_gpio_pins(void) {

    struct gpio {

        struct device *port;
        u32_t pin;
        int flags;
        u32_t state;

    };

    const array configs = {

        // LED Output Pins ---------------------------------------------

        gpio{ dev.red_led, RED_LED_PIN, (GPIO_OUTPUT), 0 },
        gpio{ dev.grn_led, GRN_LED_PIN, (GPIO_OUTPUT), 0 },

        // Button Input Pins -------------------------------------------

        gpio{ dev.red_btn, RED_BTN_PIN, (GPIO_INPUT | GPIO_ACTIVE_LOW | GPIO_PULL_UP) },
        gpio{ dev.grn_btn, GRN_BTN_PIN, (GPIO_INPUT | GPIO_ACTIVE_LOW | GPIO_PULL_UP) },

        // SSADC Pre-Configuration -------------------------------------

        gpio{ dev.gpio0,  4, (GPIO_INPUT) },

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

        gpio{ dev.gpio0,  3, (GPIO_OUTPUT), 1 }, // spi0.cs
        gpio{ dev.gpio0, 22, (GPIO_INPUT)     }, // spi0.int1
        gpio{ dev.gpio0,  2, (GPIO_INPUT)     }, // spi0.int2

        // MMC5883MA Magnetometer --------------------------------------

        // Upon power-on the INT line is held as high-z until the interrupt
        // functions are enabled, whereupon it is actively driven high and
        // low, and is active-high.

        // Note that rather than using discrete components for additional
        // I2C pull-up resistors, the pull-ups of ganged GPIO lines are used
        // to set the resistance in parallel. The nRF52 series has pull-up
        // and pull-down resistors of 11k/13k/16k min/nominal/max value,
        // so the effective pull-up is one to one-third this value,
        // depending on how many lines are used.

        gpio{ dev.gpio0,  9, (GPIO_INPUT | GPIO_ACTIVE_LOW | GPIO_PULL_UP) }, // i2c1.scl
        gpio{ dev.gpio0, 10, (GPIO_INPUT | GPIO_ACTIVE_LOW | GPIO_PULL_UP) }, // i2c1.sda
        gpio{ dev.gpio0, 12, (GPIO_INPUT | GPIO_ACTIVE_LOW | GPIO_PULL_UP) }, // i2c1.scl.pullup
        gpio{ dev.gpio0, 19, (GPIO_INPUT | GPIO_ACTIVE_LOW | GPIO_PULL_UP) }, // i2c1.scl.pullup
        gpio{ dev.gpio0, 11, (GPIO_INPUT | GPIO_ACTIVE_LOW | GPIO_PULL_UP) }, // i2c1.sda.pullup
        gpio{ dev.gpio0, 14, (GPIO_INPUT | GPIO_ACTIVE_LOW | GPIO_PULL_UP) }, // i2c1.sda.pullup
        gpio{ dev.gpio0, 26, (GPIO_INPUT)  },                                 // i2c1.int

        // -------------------------------------------------------------

    };

    for (const auto config : configs) {

        insist(gpio_pin_configure(config.port, config.pin, config.flags));

        if (config.flags & GPIO_OUTPUT) {

            insist(gpio_pin_write(config.port, config.pin, config.state));

        }

    }

    k_msleep(100); // this delay ensures that the SPI and I2C bus states are reset

}

//----------------------------------------------------------------------

static void verify_pwm_configs(void) {

    struct pwm {

        struct device *port;
        u32_t pin;

    };

    const array configs = {

        pwm{ dev.red_pwm, RED_LED_PIN },
        pwm{ dev.grn_pwm, GRN_LED_PIN },

    };

    for (const auto config : configs) {

        u64_t cycles = 0;
        insist(pwm_get_cycles_per_sec(config.port, config.pin, &cycles));

        if (unlikely(cycles != UINT64_C(16000000))) {
            sys_panic("unexpected pwm_get_cycles_per_sec: 0x%08x%08x", u32_t(cycles >> 32), u32_t(cycles));
        }

    }

}

//----------------------------------------------------------------------

static void verify_counter_configs(void) {

    struct counter {

        struct device *port;
        const char *name;
        u32_t frequency;
        bool is_counting_up;
        u8_t alarm_channels;
        u32_t top_value;

    };

    const array configs = {

        counter{ .port = dev.rtc2,   .name = "rtc2",   .frequency = rtc2_frequency,   .is_counting_up = true, .alarm_channels = 3, .top_value = 0x00FFFFFF },
        counter{ .port = dev.timer1, .name = "timer1", .frequency = timer1_frequency, .is_counting_up = true, .alarm_channels = 2, .top_value = 0xFFFFFFFF },
        counter{ .port = dev.timer2, .name = "timer2", .frequency = timer2_frequency, .is_counting_up = true, .alarm_channels = 2, .top_value = 0xFFFFFFFF },
        counter{ .port = dev.timer3, .name = "timer3", .frequency = timer3_frequency, .is_counting_up = true, .alarm_channels = 4, .top_value = 0xFFFFFFFF },
        counter{ .port = dev.timer4, .name = "timer4", .frequency = timer4_frequency, .is_counting_up = true, .alarm_channels = 4, .top_value = 0xFFFFFFFF },

    };

    for (const auto config : configs) {

        struct device *port = config.port;

        struct counter instance = {
            .port = port,
            .frequency = counter_get_frequency(port),
            .is_counting_up = counter_is_counting_up(port),
            .alarm_channels = counter_get_num_of_channels(port),
            .top_value = counter_get_top_value(port),
        };

        if (unlikely(
            (instance.port != config.port) ||
            (instance.frequency != config.frequency) ||
            (instance.is_counting_up != config.is_counting_up) ||
            (instance.alarm_channels != config.alarm_channels) ||
            (instance.top_value != config.top_value)
        )) {
            sys_panic("counter/timer misconfiguration:\n"
                      "- counter[%s] want: { frequency: %u, is_counting_up: %d, alarm_channels: %d, top_value: 0x%08x }\n"
                      "- counter[%s] have: { frequency: %u, is_counting_up: %d, alarm_channels: %d, top_value: 0x%08x }",
                      config.name, config.frequency,   config.is_counting_up,   config.alarm_channels,   config.top_value,
                      config.name, instance.frequency, instance.is_counting_up, instance.alarm_channels, instance.top_value);

        }

    }

}

//----------------------------------------------------------------------
