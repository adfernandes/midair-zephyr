#pragma once

//----------------------------------------------------------------------

#include "main.hpp"
#include "devices.hpp"

void configure_lsm6dsox(void);

const u8_t spi_orc = CONFIG_SPI_0_NRF_ORC;

static inline auto spi_write_register(const u8_t r ) -> u8_t { return r; }
static inline auto spi_read_register (const u8_t r ) -> u8_t { return UINT8_C(0x80) | r; }

auto spi_simple_transceive(struct device *dev, const struct spi_config *config, u8_t *tx_buffer, u8_t *rx_buffer, size_t length) -> int;

//----------------------------------------------------------------------
