//
// Copyright (c) 2019 Andrew Fernandes <andrew@fernandes.org>
//
// SPDX-License-Identifier: Apache-2.0
//
#include <zephyr.h>

#include <device.h>
#include <counter.h>
#include <gpio.h>
#include <spi.h>
#include <i2c.h>

// These 'include' directives are NOT needed by Zephyr.
// They are only included so that Visual Studio Code's
// "vscode-cpptools" intellisense can worl properly.
//
#if defined(VSCODE_CPPTOOLS_INTELLISENSE)
#include "zephyr/include/generated/autoconf.h"
#include "zephyr/soc/arm/nordic_nrf/nrf52/dts_fixup.h"
#include "zephyr/include/generated/generated_dts_board_unfixed.h"
#endif

#define RED_LED_CONTROLLER DT_GPIO_LEDS_LED0_RED_GPIO_CONTROLLER
#define RED_LED_PIN        DT_GPIO_LEDS_LED0_RED_GPIO_PIN

#define GRN_LED_CONTROLLER DT_GPIO_LEDS_LED1_GREEN_GPIO_CONTROLLER
#define GRN_LED_PIN        DT_GPIO_LEDS_LED1_GREEN_GPIO_PIN

#define RED_BTN_CONTROLLER DT_GPIO_KEYS_BUTTON0_RED_GPIO_CONTROLLER
#define RED_BTN_PIN        DT_GPIO_KEYS_BUTTON0_RED_GPIO_PIN

#define GRN_BTN_CONTROLLER DT_GPIO_KEYS_BUTTON1_GREEN_GPIO_CONTROLLER
#define GRN_BTN_PIN        DT_GPIO_KEYS_BUTTON1_GREEN_GPIO_PIN

static void configure_spi(void);
static void configure_i2c(void);
static void configure_buttons(void);
static void button_changed(struct device *dev, struct gpio_callback *cb, u32_t pins);

static atomic_t red_btn_state = ATOMIC_INIT(0);
static atomic_t grn_btn_state = ATOMIC_INIT(0);

static struct device *red_led_dev;
static struct device *grn_led_dev;

static struct device *gpio_dev;

static struct device *spi_dev;
static struct device *i2c_dev;

static struct spi_cs_control spi_cs = {
	.gpio_pin = DT_NORDIC_NRF_SPI_0_CS_GPIOS_PIN,
	.delay = 1, // microseconds
};

static struct spi_config spi_cfg = {
	.frequency = 8000000, // MHz
	.operation = SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_WORD_SET(8) | SPI_LINES_SINGLE,
	.cs = &spi_cs,
	.slave = 0,
};


// TODO add a 'zassert_not_null' check for every 'device_get_binding' https://github.com/zephyrproject-rtos/zephyr/blob/master/tests/drivers/adc/adc_api/src/test_adc.c


void main(void) {

    gpio_dev = device_get_binding(DT_NORDIC_NRF_GPIO_0_LABEL);

    red_led_dev = device_get_binding(RED_LED_CONTROLLER);
    gpio_pin_configure(red_led_dev, RED_LED_PIN, (GPIO_DIR_OUT | GPIO_PUD_NORMAL));

    grn_led_dev = device_get_binding(GRN_LED_CONTROLLER);
    gpio_pin_configure(grn_led_dev, GRN_LED_PIN, (GPIO_DIR_OUT | GPIO_PUD_NORMAL));

    configure_spi();
    configure_i2c();
    configure_buttons();

printk("device_get_binding(RTC_0): %p\n", device_get_binding(DT_NORDIC_NRF_RTC_RTC_0_LABEL));
printk("device_get_binding(RTC_1): %p\n", device_get_binding(DT_NORDIC_NRF_RTC_RTC_1_LABEL));
printk("device_get_binding(RTC_2): %p\n", device_get_binding(DT_NORDIC_NRF_RTC_RTC_2_LABEL));
printk("device_get_binding(TIMER_0): %p\n", device_get_binding(DT_NORDIC_NRF_TIMER_TIMER_0_LABEL));
printk("device_get_binding(TIMER_1): %p\n", device_get_binding(DT_NORDIC_NRF_TIMER_TIMER_1_LABEL));
printk("device_get_binding(TIMER_2): %p\n", device_get_binding(DT_NORDIC_NRF_TIMER_TIMER_2_LABEL));
printk("device_get_binding(TIMER_3): %p\n", device_get_binding(DT_NORDIC_NRF_TIMER_TIMER_3_LABEL));
printk("device_get_binding(TIMER_4): %p\n", device_get_binding(DT_NORDIC_NRF_TIMER_TIMER_4_LABEL));
//
struct device *rtc_dev = device_get_binding(DT_NORDIC_NRF_RTC_RTC_2_LABEL);
printk("counter frequency: %u Hz, is_counting_up: %d, alarm_channels: %u, top_value: %u\n",
    counter_get_frequency(rtc_dev), (int)counter_is_counting_up(rtc_dev),
    (unsigned)counter_get_num_of_channels(rtc_dev), counter_get_top_value(rtc_dev));
k_sleep(10); // milliseconds
struct device *timer1_dev = device_get_binding(DT_NORDIC_NRF_TIMER_TIMER_1_LABEL);
printk("counter frequency: %u Hz, is_counting_up: %d, alarm_channels: %u, top_value: %u\n",
    counter_get_frequency(timer1_dev), (int)counter_is_counting_up(timer1_dev),
    (unsigned)counter_get_num_of_channels(timer1_dev), counter_get_top_value(timer1_dev));
k_sleep(10); // milliseconds
struct device *timer2_dev = device_get_binding(DT_NORDIC_NRF_TIMER_TIMER_2_LABEL);
printk("counter frequency: %u Hz, is_counting_up: %d, alarm_channels: %u, top_value: %u\n",
    counter_get_frequency(timer2_dev), (int)counter_is_counting_up(timer2_dev),
    (unsigned)counter_get_num_of_channels(timer2_dev), counter_get_top_value(timer2_dev));
k_sleep(10); // milliseconds
struct device *timer3_dev = device_get_binding(DT_NORDIC_NRF_TIMER_TIMER_3_LABEL);
printk("counter frequency: %u Hz, is_counting_up: %d, alarm_channels: %u, top_value: %u\n",
    counter_get_frequency(timer3_dev), (int)counter_is_counting_up(timer3_dev),
    (unsigned)counter_get_num_of_channels(timer3_dev), counter_get_top_value(timer3_dev));
k_sleep(10); // milliseconds
struct device *timer4_dev = device_get_binding(DT_NORDIC_NRF_TIMER_TIMER_4_LABEL);
printk("counter frequency: %u Hz, is_counting_up: %d, alarm_channels: %u, top_value: %u\n",
    counter_get_frequency(timer4_dev), (int)counter_is_counting_up(timer4_dev),
    (unsigned)counter_get_num_of_channels(timer4_dev), counter_get_top_value(timer4_dev));
k_sleep(10); // milliseconds

    u32_t count = 0;
    counter_start(rtc_dev);

    while (true) {

        const bool current_red_btn_state = (bool)atomic_get(&red_btn_state);
        const bool current_grn_btn_state = (bool)atomic_get(&grn_btn_state);

        printk("count: %u, rtc: 0x%08x, red: %u, green: %u\n",
            count, counter_read(rtc_dev), current_red_btn_state, current_grn_btn_state);

        gpio_pin_write(red_led_dev, RED_LED_PIN,  (count & 1) || current_red_btn_state);
        gpio_pin_write(grn_led_dev, GRN_LED_PIN, !(count & 1) || current_grn_btn_state);

        k_sleep(1000); // milliseconds
        count++;

    }

}


static void configure_buttons(void) {

	static struct gpio_callback button_cb;

	struct device *red_btn_dev = device_get_binding(RED_BTN_CONTROLLER);
	gpio_pin_configure(red_btn_dev, RED_BTN_PIN, (GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_INT | GPIO_INT_EDGE | GPIO_INT_DOUBLE_EDGE));

	struct device *grn_btn_dev = device_get_binding(GRN_BTN_CONTROLLER);
	gpio_pin_configure(grn_btn_dev, GRN_BTN_PIN, (GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_INT | GPIO_INT_EDGE | GPIO_INT_DOUBLE_EDGE));

    gpio_init_callback(&button_cb, button_changed, BIT(RED_BTN_PIN) | BIT(GRN_BTN_PIN));

    gpio_add_callback(red_btn_dev, &button_cb);
    gpio_add_callback(grn_btn_dev, &button_cb);

	gpio_pin_enable_callback(red_btn_dev, RED_BTN_PIN);
	gpio_pin_enable_callback(grn_btn_dev, GRN_BTN_PIN);

}


static void button_changed(struct device *dev, struct gpio_callback *cb, u32_t pins) {

    static char *state[2] = { "released", "pressed" };

    if (pins & BIT(RED_BTN_PIN)) {

        u32_t value;
        gpio_pin_read(dev, RED_BTN_PIN, &value);
        value = value ? 0 : 1; // negated

        gpio_pin_write(red_led_dev, RED_LED_PIN, value);
        atomic_set(&red_btn_state, value);

        printk("red: %s\n", state[value]);

    }

    if (pins & BIT(GRN_BTN_PIN)) {

        u32_t value;
        gpio_pin_read(dev, GRN_BTN_PIN, &value);
        value = value ? 0 : 1; // negated

        gpio_pin_write(grn_led_dev, GRN_LED_PIN, value);
        atomic_set(&grn_btn_state, value);

        printk("green: %s\n", state[value]);

    }

}


static void configure_spi(void) {

    // TODO: Bring CS high via zero-length xfer, wait, then reset the device, then wait

    printk("INIT | spi_dev: %p, spi_cs.gpio_dev: %p\n", spi_dev, spi_cs.gpio_dev);

    spi_dev = device_get_binding(DT_SPI_0_NAME);
    spi_cs.gpio_dev = device_get_binding(DT_NORDIC_NRF_SPI_0_CS_GPIOS_CONTROLLER);

    printk("FINI | spi_dev: %p, spi_cs.gpio_dev: %p\n", spi_dev, spi_cs.gpio_dev);

    const u8_t who_am_i_read_register = 0x80 | 0x0F;
    const u8_t lsm6dsox = 0x6C;

    u8_t tx_buffer[] = { who_am_i_read_register, CONFIG_SPI_0_NRF_ORC };
    u8_t rx_buffer[] = { 0, 0 };

    const struct spi_buf tx_buffers[] = {
		{
			.buf = tx_buffer,
			.len = ARRAY_SIZE(tx_buffer)
		},
	};
	const struct spi_buf rx_buffers[] = {
		{
			.buf = rx_buffer,
			.len = ARRAY_SIZE(rx_buffer)
		},
	};
	const struct spi_buf_set tx = {
		.buffers = tx_buffers,
		.count = ARRAY_SIZE(tx_buffers)
	};
	const struct spi_buf_set rx = {
		.buffers = rx_buffers,
		.count = ARRAY_SIZE(rx_buffers)
	};

    // TODO: BOOT and SW_RESET on Register CTRL3_C (12h)

    int rv = spi_transceive(spi_dev, &spi_cfg, &tx, &rx);

    printk("spi_transceive: %d, rx_data: 0x%02X -> %s\n", rv, rx_buffer[1],
        rx_buffer[1] == lsm6dsox ? "SUCCESS" : "FAILURE");

}


static void configure_i2c(void) {

    const u32_t i2c_pullup_pin[] = { 9, 10, 11, 12, 14, 19 };

    for (u32_t i = 0; i < ARRAY_SIZE(i2c_pullup_pin); i++) {
        gpio_pin_configure(gpio_dev, i2c_pullup_pin[i], (GPIO_DIR_IN | GPIO_PUD_PULL_UP));
    }

    k_sleep(2); // milliseconds

    // TODO: Fully reset the device

    const u16_t mmc5883ma_i2c_addr = 0b0110000;

    const u8_t mmc5883ma_product_id1_register = 0x2F;
    const u8_t mmc5883ma_product_id1_reset_value = 0b00001100;


    const u32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_MASTER;

    i2c_dev = device_get_binding(DT_I2C_1_NAME);

    printk("i2c_dev: %p, ", i2c_dev);
    if (!i2c_configure(i2c_dev, i2c_cfg)) {
		printk("config: succeeded\n");
	}

    u8_t rx_buffer[1] = { 0 };
    int rv = i2c_burst_read(i2c_dev, mmc5883ma_i2c_addr, mmc5883ma_product_id1_register, rx_buffer, 1);

    printk("i2c_burst_read: %d, rx_data: 0x%02X -> %s\n", rv, rx_buffer[0],
        rx_buffer[0] == mmc5883ma_product_id1_reset_value ? "SUCCESS" : "FAILURE");

}
