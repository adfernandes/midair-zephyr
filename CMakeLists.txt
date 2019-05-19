#
# Copyright (c) 2019 Andrew Fernandes <andrew@fernandes.org>
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.14.4)

set_property(GLOBAL PROPERTY CSTD "gnu11")

set(CMAKE_BUILD_TYPE "Debug") # Debug, Release, RelWithDebInfo, MinSizeRel

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)

project(midair)

file(GLOB C_SOURCES src/*.c)

target_sources(app PRIVATE ${C_SOURCES})
