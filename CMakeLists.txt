#
# Copyright (c) 2019 Andrew Fernandes <andrew@fernandes.org>
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.14.4)

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

file(GLOB C_SOURCES   "src/*.c")
file(GLOB CXX_SOURCES "src/*.cpp")

target_sources(app PRIVATE ${C_SOURCES} ${CXX_SOURCES})
