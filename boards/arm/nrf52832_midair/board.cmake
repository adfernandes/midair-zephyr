#
# Copyright (c) 2019 Andrew Fernandes <andrew@fernandes.org>
#
# SPDX-License-Identifier: Apache-2.0
#

set(JLINK_SERIAL_NUMBER "260114307")

board_runner_args(nrfjprog "--nrf-family=NRF52" "--erase" "--snr" "${JLINK_SERIAL_NUMBER}")
board_runner_args(jlink "--device=nrf52" "--snr=${JLINK_SERIAL_NUMBER}")
board_runner_args(pyocd "--target=nrf52832")

include(${ZEPHYR_BASE}/boards/common/nrfjprog.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
