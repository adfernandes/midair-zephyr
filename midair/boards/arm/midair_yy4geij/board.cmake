# midair-yy4geij configuration
#
# Copyright (c) 2020 Andrew Fernandes <andrew@fernandes.org>
# SPDX-License-Identifier: Apache-2.0

board_runner_args(nrfjprog "--nrf-family=NRF52") # "--erase" "--snr" "${JLINK_SERIAL_NUMBER}"
board_runner_args(jlink "--device=nrf52" "--speed=8000") # "--snr=${JLINK_SERIAL_NUMBER}"
board_runner_args(pyocd "--target=nrf52832" "--frequency=8000000")

include(${ZEPHYR_BASE}/boards/common/nrfjprog.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
