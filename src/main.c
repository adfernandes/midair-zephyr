//
// Copyright (c) 2019 Andrew Fernandes <andrew@fernandes.org>
//
// SPDX-License-Identifier: Apache-2.0
//
#include "main.h"

static void configure_spi(void);
static void configure_i2c(void);
static void configure_buttons(void);
static void button_changed(struct device *dev, struct gpio_callback *cb, u32_t pins);

static atomic_t red_btn_state = ATOMIC_INIT(0);
static atomic_t grn_btn_state = ATOMIC_INIT(0);

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

LOG_MODULE_REGISTER(midair, LOG_LEVEL_DBG);

void main(void) {

    devices_init();

    gpio_pin_configure(dev.red_led, RED_LED_PIN, (GPIO_DIR_OUT | GPIO_PUD_NORMAL));
    gpio_pin_configure(dev.grn_led, GRN_LED_PIN, (GPIO_DIR_OUT | GPIO_PUD_NORMAL));

    configure_spi();
    configure_i2c();
    configure_buttons();

LOG_INF("counter frequency: %u Hz, is_counting_up: %d, alarm_channels: %u, top_value: %u",
    counter_get_frequency(dev.rtc2), (int)counter_is_counting_up(dev.rtc2),
    (unsigned)counter_get_num_of_channels(dev.rtc2), counter_get_top_value(dev.rtc2));
k_sleep(10); // milliseconds
LOG_INF("counter frequency: %u Hz, is_counting_up: %d, alarm_channels: %u, top_value: %u",
    counter_get_frequency(dev.timer1), (int)counter_is_counting_up(dev.timer1),
    (unsigned)counter_get_num_of_channels(dev.timer1), counter_get_top_value(dev.timer1));
k_sleep(10); // milliseconds
LOG_INF("counter frequency: %u Hz, is_counting_up: %d, alarm_channels: %u, top_value: %u",
    counter_get_frequency(dev.timer2), (int)counter_is_counting_up(dev.timer2),
    (unsigned)counter_get_num_of_channels(dev.timer2), counter_get_top_value(dev.timer2));
k_sleep(10); // milliseconds
LOG_INF("counter frequency: %u Hz, is_counting_up: %d, alarm_channels: %u, top_value: %u",
    counter_get_frequency(dev.timer3), (int)counter_is_counting_up(dev.timer3),
    (unsigned)counter_get_num_of_channels(dev.timer3), counter_get_top_value(dev.timer3));
k_sleep(10); // milliseconds
LOG_INF("counter frequency: %u Hz, is_counting_up: %d, alarm_channels: %u, top_value: %u",
    counter_get_frequency(dev.timer4), (int)counter_is_counting_up(dev.timer4),
    (unsigned)counter_get_num_of_channels(dev.timer4), counter_get_top_value(dev.timer4));
k_sleep(10); // milliseconds

    u32_t count = 0;
    counter_start(dev.rtc2);

    while (true) {

        const bool current_red_btn_state = (bool)atomic_get(&red_btn_state);
        const bool current_grn_btn_state = (bool)atomic_get(&grn_btn_state);

        LOG_DBG("count: %u, rtc: 0x%08x, red: %u, green: %u",
            count, counter_read(dev.rtc2), current_red_btn_state, current_grn_btn_state);

        gpio_pin_write(dev.red_led, RED_LED_PIN,  (count & 1) || current_red_btn_state);
        gpio_pin_write(dev.grn_led, GRN_LED_PIN, !(count & 1) || current_grn_btn_state);

        k_sleep(1000); // milliseconds
        count++;

    }

}


static void configure_buttons(void) {

	static struct gpio_callback button_cb;

	gpio_pin_configure(dev.red_btn, RED_BTN_PIN, (GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_INT | GPIO_INT_EDGE | GPIO_INT_DOUBLE_EDGE));
    gpio_pin_configure(dev.grn_btn, GRN_BTN_PIN, (GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_INT | GPIO_INT_EDGE | GPIO_INT_DOUBLE_EDGE));

    gpio_init_callback(&button_cb, button_changed, BIT(RED_BTN_PIN) | BIT(GRN_BTN_PIN));

    gpio_add_callback(dev.red_btn, &button_cb);
    gpio_add_callback(dev.grn_btn, &button_cb);

	gpio_pin_enable_callback(dev.red_btn, RED_BTN_PIN);
	gpio_pin_enable_callback(dev.grn_btn, GRN_BTN_PIN);

}


static void button_changed(struct device *dev_btn, struct gpio_callback *cb, u32_t pins) {

    static char *state[2] = { "released", "pressed" };

    if (pins & BIT(RED_BTN_PIN)) {

        u32_t value;
        gpio_pin_read(dev_btn, RED_BTN_PIN, &value);
        value = value ? 0 : 1; // negated

        gpio_pin_write(dev.red_led, RED_LED_PIN, value);
        atomic_set(&red_btn_state, value);

        LOG_INF("red: %s", state[value]);

    }

    if (pins & BIT(GRN_BTN_PIN)) {

        u32_t value;
        gpio_pin_read(dev_btn, GRN_BTN_PIN, &value);
        value = value ? 0 : 1; // negated

        gpio_pin_write(dev.grn_led, GRN_LED_PIN, value);
        atomic_set(&grn_btn_state, value);

        LOG_INF("green: %s", state[value]);

    }

}


static void configure_spi(void) {

    // TODO: Bring CS high via zero-length xfer, wait, then reset the device, then wait

    spi_cs.gpio_dev = dev.spi0cs;

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

    int rv = spi_transceive(dev.spi0, &spi_cfg, &tx, &rx);

    LOG_INF("spi_transceive: %d, rx_data: 0x%02X -> %s", rv, rx_buffer[1],
        rx_buffer[1] == lsm6dsox ? "SUCCESS" : "FAILURE");

}


static void configure_i2c(void) {

    const u32_t i2c_pullup_pin[] = { 9, 10, 11, 12, 14, 19 };

    for (u32_t i = 0; i < ARRAY_SIZE(i2c_pullup_pin); i++) {
        gpio_pin_configure(dev.gpio0, i2c_pullup_pin[i], (GPIO_DIR_IN | GPIO_PUD_PULL_UP));
    }

    k_sleep(2); // milliseconds

    // TODO: Fully reset the device

    const u16_t mmc5883ma_i2c_addr = 0b0110000;

    const u8_t mmc5883ma_product_id1_register = 0x2F;
    const u8_t mmc5883ma_product_id1_reset_value = 0b00001100;


    const u32_t i2c1_cfg = I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_MASTER;

    const bool i2c1_configured = !i2c_configure(dev.i2c1, i2c1_cfg);
    LOG_DBG("dev.i2c1: %p, configured: %s", dev.i2c1, i2c1_configured ? "TRUE" : "FALSE");

    u8_t rx_buffer[1] = { 0 };
    int rv = i2c_burst_read(dev.i2c1, mmc5883ma_i2c_addr, mmc5883ma_product_id1_register, rx_buffer, 1);

    LOG_INF("i2c_burst_read: %d, rx_data: 0x%02X -> %s", rv, rx_buffer[0],
        rx_buffer[0] == mmc5883ma_product_id1_reset_value ? "SUCCESS" : "FAILURE");

}
