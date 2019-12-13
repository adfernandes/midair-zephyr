#pragma once

//----------------------------------------------------------------------
// Core include files

#include <zephyr.h>

#include <logging/log.h>
#include <logging/log_ctrl.h>

#include <power/power.h>

#include <device.h>
#include <counter.h>
#include <gpio.h>
#include <spi.h>
#include <i2c.h>
#include <adc.h>
#include <pwm.h>

#ifdef CONFIG_BT
#include "ble.hpp"
#endif

//----------------------------------------------------------------------
// Nordic specific drivers and HAL subsystems

#include <nrfx.h>

#include <nrfx_ppi.h>
#include <nrfx_gpiote.h>
#include <nrfx_saadc.h>

#include <nrfx/hal/nrf_ficr.h>
#include <nrfx/hal/nrf_clock.h>
#include <nrfx/hal/nrf_saadc.h>
#include <nrfx/hal/nrf_radio.h>
#include <nrfx/hal/nrf_systick.h>

#include <lsm6dsox_reg.h>

//----------------------------------------------------------------------
// Standard C and C++ headers

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

extern "C" char *gcvtf(float, int, char *); // newlib: float -> string

#include <algorithm>
#include <type_traits>
#include <array>

using namespace std;

//----------------------------------------------------------------------
// A C++14 safe way of casting an 'enum class' to it's underlying type,
// see https://stackoverflow.com/a/33083231/1229371 for details

template <typename E>
constexpr auto to_underlying(E e) noexcept;

template<typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

//----------------------------------------------------------------------
// Static and dynamic assertion and verification macros and functions

#include "insist.h" // valid for C and C++ both

//----------------------------------------------------------------------
// These 'include' directives are NOT needed by Zephyr.
//
// They are only included so that Visual Studio Code's
// "vscode-cpptools" intellisense can work properly.

#if defined(VSCODE_CPPTOOLS_INTELLISENSE)
#include "zephyr/include/generated/autoconf.h"
#include "zephyr/soc/arm/nordic_nrf/nrf52/dts_fixup.h"
#include "zephyr/include/generated/generated_dts_board_unfixed.h"
#define  k_panic() // this expands properly during compilation
#define  k_oops()  // this expands properly during compilation
#endif

//----------------------------------------------------------------------
