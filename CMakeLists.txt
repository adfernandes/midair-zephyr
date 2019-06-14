#
# Copyright (c) 2019 Andrew Fernandes <andrew@fernandes.org>
#
# SPDX-License-Identifier: Apache-2.0
#
cmake_minimum_required(VERSION 3.14.5)

set_property(GLOBAL PROPERTY CSTD "gnu17") # https://gcc.gnu.org/onlinedocs/gcc/Standards.html

set(CMAKE_BUILD_TYPE "Debug") # One of: Debug, Release, RelWithDebInfo, MinSizeRel

# https://docs.zephyrproject.org/latest/guides/modules.html#without-west
#
# See the list of current modules in "${CMAKE_SOURCE_DIR}/zephyr/west.yml"
#
set(ZEPHYR_MODULES "${CMAKE_SOURCE_DIR}/modules/segger")
#
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)

project(midair)

include_directories(
    "src"
    "src/dev"
    "src/ble"
)

set(SOURCES
    "src/main.cpp"
    "src/dev/devices.cpp"
    "src/dev/leds.cpp"
    "src/dev/buttons.cpp"
    "src/dev/battery.cpp"
    "src/dev/lsm6dsox.cpp"
    "src/dev/mmc5883ma.cpp"
)

target_sources(app PRIVATE ${SOURCES})
