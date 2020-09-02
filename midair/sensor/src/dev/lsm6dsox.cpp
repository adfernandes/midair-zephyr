#include "lsm6dsox.hpp"

#include "lsm6dsox_reg.h"

LOG_MODULE_DECLARE(midair, LOG_LEVEL_DBG);

//----------------------------------------------------------------------

static struct spi_cs_control spi_cs = {
    .gpio_dev = nullptr, // FIXME &&& DT_GPIO_LABEL(DT_NODELABEL(spi0), cs_gpios),
    .delay = 0, // in microseconds
    .gpio_pin = DT_GPIO_PIN(DT_NODELABEL(spi0), cs_gpios),
    .gpio_dt_flags = DT_GPIO_FLAGS(DT_NODELABEL(spi0), cs_gpios),
};

static struct spi_config spi_cfg = {
	.frequency = 8000000, // MHz // FIXME Set this from the device-tree
	.operation = SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_WORD_SET(8) | SPI_LINES_SINGLE,
	.slave = 0,
	.cs = &spi_cs,
};

//----------------------------------------------------------------------

int spi_simple_transceive(struct device *dev, const struct spi_config *config, uint8_t *tx_buffer, uint8_t *rx_buffer, size_t length) {

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

static int32_t stmdev_write(void *handle, uint8_t reg, uint8_t *data, uint16_t length) { return 0; } // FIXME Start Here
static int32_t stmdev_read (void *handle, uint8_t reg, uint8_t *data, uint16_t length) { return 0; } // FIXME Start Here

static const stmdev_ctx_t stmdev_ctx = {
    .write_reg = stmdev_write,
    .read_reg  = stmdev_read,
    .handle = nullptr,
};

//----------------------------------------------------------------------

void configure_lsm6dsox(void) {

    spi_cs.gpio_dev = dev.spi0cs;

    const uint8_t lsm6dsox_ctrl3_c_register = spi_write_register(0x12);
    const uint8_t lsm6dsox_ctrl3_c_boot_and_sw_reset_value = 0x81;

    const uint8_t lsm6dsox_who_am_i_register = spi_read_register(0x0F);
    const uint8_t lsm6dsox_who_am_i_reply_value = 0x6C;

    array tx_buffer = { lsm6dsox_ctrl3_c_register, lsm6dsox_ctrl3_c_boot_and_sw_reset_value };
    array rx_buffer = { uint8_t(0), uint8_t(0) };

    STATIC_ASSERT(tx_buffer.size() == rx_buffer.size());
    const size_t length = tx_buffer.size();

    LOG_DBG("initiating boot and sw_reset from spi idle");

    insist(spi_simple_transceive(dev.spi0, &spi_cfg, tx_buffer.data(), rx_buffer.data(), length));

    k_msleep(20); // the reboot time fo the LSM6DSOX is 10 milliseconds

    tx_buffer[0] = lsm6dsox_who_am_i_register; tx_buffer[1] = spi_orc;
    rx_buffer[0] = 0; rx_buffer[1] = 0;

    insist(spi_simple_transceive(dev.spi0, &spi_cfg, tx_buffer.data(), rx_buffer.data(), length));

    if (unlikely(rx_buffer[1] != lsm6dsox_who_am_i_reply_value)) {
        sys_panic("spi_simple_transceive: lsm6dsox_who_am_i_register != lsm6dsox_who_am_i_reply_value");
    }

    LOG_DBG("who_am_i register value read and verified");

}

//----------------------------------------------------------------------
