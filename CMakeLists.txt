# nrf52832-midair configuration
#
# Copyright (c) 2020 Andrew Fernandes <andrew@fernandes.org>
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.17.1)

# -----------------------------------------------------------------------------

# Set the 'BOARD_ROOT' property for CLion
set(BOARD_ROOT "$ENV{PROJECT_DIR}")

# The C++ Standard version is set in 'prj.conf' via the 'CONFIG_STD_CPP17' setting
#
set_property(GLOBAL PROPERTY CSTD "gnu17") # https://gcc.gnu.org/onlinedocs/gcc/Standards.html

# https://docs.zephyrproject.org/latest/guides/modules.html#without-west
#
# See the list of current modules in "${CMAKE_SOURCE_DIR}/zephyr/west.yml"
#
set(ZEPHYR_MODULES
    "${CMAKE_SOURCE_DIR}/modules/cmsis"
    "${CMAKE_SOURCE_DIR}/modules/nordic"
    "${CMAKE_SOURCE_DIR}/modules/segger"
    "${CMAKE_SOURCE_DIR}/modules/stmemsc"
)
#
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)

# -----------------------------------------------------------------------------

project(midair)

include_directories(
    "src"
    "src/dev"
    "src/ble"
)

set(CXX_SOURCES
    "src/main.cpp"
    "src/dev/devices.cpp"
    "src/dev/leds.cpp"
    "src/dev/buttons.cpp"
    "src/dev/battery.cpp"
    "src/dev/lsm6dsox.cpp"
    "src/dev/mmc5883ma.cpp"
)

target_sources(app PRIVATE ${CXX_SOURCES})

# -----------------------------------------------------------------------------
# Done
