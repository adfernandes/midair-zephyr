# midair sensor configuration
#
# Copyright (c) 2020 Andrew Fernandes <andrew@fernandes.org>
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION "$ENV{CMAKE_MINIMUM_REQUIRED}")

# -----------------------------------------------------------------------------

# We might use a custom board
#
set(BOARD "midair_yy4geij")
#
set(BOARD_ROOT "$ENV{PROJECT_DIR}/midair")

# The C++ Standard version is set in 'prj.conf' via the 'CONFIG_STD_CPP17' setting
#
set_property(GLOBAL PROPERTY CSTD "gnu17") # https://gcc.gnu.org/onlinedocs/gcc/Standards.html

# https://docs.zephyrproject.org/latest/guides/modules.html#without-west
#
# See the list of current modules in "$ENV{PROJECT_DIR}/zephyr/west.yml"
#
set(ZEPHYR_MODULES
    "$ENV{PROJECT_DIR}/modules/cmsis"
    "$ENV{PROJECT_DIR}/modules/nordic"
    "$ENV{PROJECT_DIR}/modules/segger"
    "$ENV{PROJECT_DIR}/modules/stmemsc"
)
#
include("$ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake" NO_POLICY_SCOPE)

# -----------------------------------------------------------------------------

project(sensor)

include_directories(
    "src"
    "src/dev"
    "src/ble"
)

set(CXX_SOURCES
    "src/new.cpp"
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
