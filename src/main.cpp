//
// Copyright (c) 2019 Andrew Fernandes <andrew@fernandes.org>
//
// SPDX-License-Identifier: Apache-2.0
//
#include "main.hpp"
#include "init.hpp"

static void configure_lsm6dsox(void);
static void configure_mmc5883ma(void);

static void configure_buttons(void);
static void button_changed(struct device *dev, struct gpio_callback *cb, u32_t pins);

static struct spi_cs_control spi_cs = {
	.gpio_pin = DT_NORDIC_NRF_SPI_0_CS_GPIOS_PIN,
	.delay = 0, // microseconds
};

static struct spi_config spi_cfg = {
	.frequency = 8000000, // MHz
	.operation = SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_WORD_SET(8) | SPI_LINES_SINGLE,
	.slave = 0,
	.cs = &spi_cs,
};

LOG_MODULE_REGISTER(midair, LOG_LEVEL_DBG);

void main(void) {

    devices_init();

    configure_lsm6dsox();
    configure_mmc5883ma();
    configure_buttons();

    u32_t count = 0;
    counter_start(dev.rtc2);

    while (true) {

        const bool current_red_btn_state = get_red_btn_state();
        const bool current_grn_btn_state = get_grn_btn_state();

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

	gpio_pin_configure(dev.red_btn, RED_BTN_PIN, (GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_INT | GPIO_INT_DEBOUNCE | GPIO_INT_EDGE | GPIO_INT_DOUBLE_EDGE));
    gpio_pin_configure(dev.grn_btn, GRN_BTN_PIN, (GPIO_DIR_IN | GPIO_PUD_PULL_UP | GPIO_INT | GPIO_INT_DEBOUNCE | GPIO_INT_EDGE | GPIO_INT_DOUBLE_EDGE));

    gpio_init_callback(&button_cb, button_changed, BIT(RED_BTN_PIN) | BIT(GRN_BTN_PIN));

    gpio_add_callback(dev.red_btn, &button_cb);
    gpio_add_callback(dev.grn_btn, &button_cb);

	gpio_pin_enable_callback(dev.red_btn, RED_BTN_PIN);
	gpio_pin_enable_callback(dev.grn_btn, GRN_BTN_PIN);

}


static void button_changed(struct device *dev_btn, struct gpio_callback *cb, u32_t pins) {

    static const char * const state[2] = { "released", "pressed" };

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


static void configure_lsm6dsox(void) {

    // TODO: BOOT and SW_RESET on Register CTRL3_C (12h)

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

    int rv = spi_transceive(dev.spi0, &spi_cfg, &tx, &rx);

    LOG_INF("spi_transceive: %d, rx_data: 0x%02X -> %s", rv, rx_buffer[1],
        rx_buffer[1] == lsm6dsox ? "SUCCESS" : "FAILURE");

}


static void configure_mmc5883ma(void) {

    int failed = true;

    const u16_t mmc5883ma_i2c_addr = 0b0110000;

    const u8_t mmc5883ma_internal_control_1_register = 0x09;
    const u8_t mmc5883ma_internal_control_1_sw_reset_value = 0x80;

    const u8_t mmc5883ma_product_id_register = 0x2F;
    const u8_t mmc5883ma_product_id_reset_value = 0b00001100;

    const u32_t i2c1_cfg = (I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_MASTER);

    failed = i2c_configure(dev.i2c1, i2c1_cfg);
    if (unlikely(failed)) {
        LOG_ERR("i2c_configure: failed");
        k_panic();
    }

    failed = i2c_burst_write(dev.i2c1, mmc5883ma_i2c_addr, mmc5883ma_internal_control_1_register, &mmc5883ma_internal_control_1_sw_reset_value, 1);
    if (unlikely(failed)) {
        LOG_ERR("i2c_burst_write: mmc5883ma_internal_control_1(sw_reset) failed");
        k_panic();
    }

    k_sleep(10); // milliseconds, the MMC5883MA SW_RESET time is 5 milliseconds

    u8_t rx_buffer[1] = { 0 };
    failed = i2c_burst_read(dev.i2c1, mmc5883ma_i2c_addr, mmc5883ma_product_id_register, rx_buffer, 1);
    if (unlikely(failed)) {
        LOG_ERR("i2c_burst_read: mmc5883ma_product_id_register failed");
        k_panic();
    }
    if (unlikely(rx_buffer[0] != mmc5883ma_product_id_reset_value)) {
        LOG_ERR("i2c_burst_read: mmc5883ma_product_id_register != mmc5883ma_product_id_reset_value");
        k_panic();
    }

}
