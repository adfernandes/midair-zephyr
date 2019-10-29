#
# Copyright (c) 2019 Andrew Fernandes <andrew@fernandes.org>
#
# SPDX-License-Identifier: Apache-2.0
#
cmake_minimum_required(VERSION 3.15.4)

set(CMAKE_BUILD_TYPE "Debug") # One of: Debug, Release, RelWithDebInfo, MinSizeRel

# -----------------------------------------------------------------------------

set(USE_PRESETS True CACHE BOOL "Use the 'CMakeLists.txt' presets")

if(USE_PRESETS)

    message("Using the 'CMakeLists.txt' presets for CMake")

    set(ENV{PROJECT_DIR} "$ENV{HOME}/Projects/Nordic/midair/zephyr")
    set(ENV{ZEPHYR_BASE} "$ENV{PROJECT_DIR}/zephyr")

    set(ENV{ZEPHYR_TOOLCHAIN_VARIANT} "zephyr")
    set(ENV{ZEPHYR_SDK_INSTALL_DIR}   "$ENV{HOME}/Developer/Toolchains/zephyr-sdk-ng")

    unset(ENV{ZEPHYR_MODULES})
    set(ENV{WEST_DIR} "$ENV{PROJECT_DIR}/.west/west")

    set(BOARD      "nrf52832_midair")
    set(BOARD_ROOT "$ENV{PROJECT_DIR}")

    set(ENV{PATH} "/opt/conda/envs/zephyr/bin:$ENV{PATH}")
    set(PYTHON_EXECUTABLE "/opt/conda/envs/zephyr/bin/python")

    set(ENV{PATH} "$ENV{HOME}/Developer/Nordic/Tools/Latest/mergehex:$ENV{PATH}")
    set(ENV{PATH} "$ENV{HOME}/Developer/Nordic/Tools/Latest/nrfjprog:$ENV{PATH}")
    set(ENV{PATH} "$ENV{PROJECT_DIR}/zephyr/scripts:$ENV{PATH}")

endif(USE_PRESETS)

# -----------------------------------------------------------------------------

# The C++ Standard version is set in 'prj.conf' via the 'CONFIG_STD_CPP17' setting
#
set_property(GLOBAL PROPERTY CSTD "gnu17") # https://gcc.gnu.org/onlinedocs/gcc/Standards.html

# https://docs.zephyrproject.org/latest/guides/modules.html#without-west
#
# See the list of current modules in "${CMAKE_SOURCE_DIR}/zephyr/west.yml"
#
set(ZEPHYR_MODULES
    "${CMAKE_SOURCE_DIR}/modules/nordic"
    "${CMAKE_SOURCE_DIR}/modules/segger"
)
#
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)

# -----------------------------------------------------------------------------

project(midair)

add_compile_definitions(NRF52_PAN_74) # nRF52832 SAADC: False EVENTS_STARTED if TACQ <= 5 us

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

# -----------------------------------------------------------------------------
# Done
