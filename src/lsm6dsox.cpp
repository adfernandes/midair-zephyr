#include "lsm6dsox.hpp"

LOG_MODULE_DECLARE(midair, LOG_LEVEL_DBG);

//----------------------------------------------------------------------

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

//----------------------------------------------------------------------

int spi_simple_transceive(struct device *dev, const struct spi_config *config, u8_t *tx_buffer, u8_t *rx_buffer, size_t length) {

    const struct spi_buf tx_buffers[] = {
		{
			.buf = tx_buffer,
			.len = length
		},
	};

	const struct spi_buf rx_buffers[] = {
		{
			.buf = rx_buffer,
			.len = length
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

    return spi_transceive(dev, config, &tx, &rx);

}

//----------------------------------------------------------------------

void configure_lsm6dsox(void) {

    int failed = true;

    spi_cs.gpio_dev = dev.spi0cs;

    const u8_t lsm6dsox_ctrl3_c_register = spi_write_register(0x12);
    const u8_t lsm6dsox_ctrl3_c_boot_and_sw_reset_value = 0x81;

    const u8_t lsm6dsox_who_am_i_register = spi_read_register(0x0F);
    const u8_t lsm6dsox_who_am_i_reply_value = 0x6C;

    u8_t tx_buffer[] = { lsm6dsox_ctrl3_c_register, lsm6dsox_ctrl3_c_boot_and_sw_reset_value };
    u8_t rx_buffer[] = { 0, 0 };

    STATIC_ASSERT(ARRAY_SIZE(tx_buffer) == ARRAY_SIZE(rx_buffer));
    const size_t length = ARRAY_SIZE(tx_buffer);

    LOG_DBG("initiating boot and sw_reset from spi idle");

    failed = spi_simple_transceive(dev.spi0, &spi_cfg, tx_buffer, rx_buffer, length);
    if (unlikely(failed)) {
        LOG_ERR("spi_simple_transceive: boot and sw_reset failed");
        sys_panic();
    }

    k_sleep(10); // milliseconds, should be adequate for the device to reset

    tx_buffer[0] = lsm6dsox_who_am_i_register; tx_buffer[1] = spi_orc;
    rx_buffer[0] = 0; rx_buffer[1] = 0;

    failed = spi_simple_transceive(dev.spi0, &spi_cfg, tx_buffer, rx_buffer, length);
    if (unlikely(failed)) {
        LOG_ERR("spi_simple_transceive: who_am_i read failed");
        sys_panic();
    }
    if (unlikely(rx_buffer[1] != lsm6dsox_who_am_i_reply_value)) {
        LOG_ERR("spi_simple_transceive: lsm6dsox_who_am_i_register != lsm6dsox_who_am_i_reply_value");
        sys_panic();
    }

    LOG_DBG("who_am_i register value read and verified");

}

//----------------------------------------------------------------------
