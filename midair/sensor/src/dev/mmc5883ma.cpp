#include "mmc5883ma.hpp"

LOG_MODULE_DECLARE(midair, LOG_LEVEL_DBG);

//----------------------------------------------------------------------

void configure_mmc5883ma(void) {

    const uint16_t mmc5883ma_i2c_addr = 0b0110000;

    const uint8_t mmc5883ma_internal_control_1_register = 0x09;
    const uint8_t mmc5883ma_internal_control_1_sw_reset_value = 0x80;

    const uint8_t mmc5883ma_product_id_register = 0x2F;
    const uint8_t mmc5883ma_product_id_reset_value = 0b00001100;

    const uint32_t i2c1_cfg = (I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_MASTER);

    LOG_DBG("initiating config from i2c idle");

    insist(i2c_configure(dev.i2c1, i2c1_cfg));

    LOG_DBG("i2c bus configuration succeeded");

    insist(i2c_burst_write(dev.i2c1, mmc5883ma_i2c_addr, mmc5883ma_internal_control_1_register, &mmc5883ma_internal_control_1_sw_reset_value, 1));

    k_msleep(10); // the MMC5883MA SW_RESET time is 5 milliseconds

    LOG_DBG("sw_reset initiated and completed");

    array rx_buffer = { uint8_t(0) };
    insist(i2c_burst_read(dev.i2c1, mmc5883ma_i2c_addr, mmc5883ma_product_id_register, rx_buffer.data(), 1));

    if (unlikely(rx_buffer[0] != mmc5883ma_product_id_reset_value)) {
        sys_panic("i2c_burst_read: mmc5883ma_product_id_register != mmc5883ma_product_id_reset_value");
    }

    LOG_DBG("product_id read and verified");

}

//----------------------------------------------------------------------
