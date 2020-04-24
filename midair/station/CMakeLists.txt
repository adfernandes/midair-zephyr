# midair station configuration
#
# Copyright (c) 2020 Andrew Fernandes <andrew@fernandes.org>
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION "$ENV{CMAKE_MINIMUM_REQUIRED}")

# -----------------------------------------------------------------------------

# We might use a custom board
#
set(BOARD "nrf52840_dongle")
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
)
#
include("$ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake" NO_POLICY_SCOPE)

# -----------------------------------------------------------------------------

project(station)

include_directories(
    "src"
)

set(C_SOURCES
    "src/main.c"
)

target_sources(app PRIVATE ${C_SOURCES})

# -----------------------------------------------------------------------------
# Done
