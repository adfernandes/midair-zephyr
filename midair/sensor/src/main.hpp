#pragma once

//----------------------------------------------------------------------
// Core include files

#include <zephyr.h>

#include <device.h>
#include <devicetree_legacy_unfixed.h> // FIXME Remove me!



#include <power/power.h>

#include <drivers/counter.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <drivers/i2c.h>
#include <drivers/pwm.h>

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
#include <nrfx/hal/nrf_radio.h>
#include <nrfx/hal/nrf_systick.h>

#include <lsm6dsox_reg.h>

//----------------------------------------------------------------------
// Standard C and C++ headers

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include <algorithm>
#include <type_traits>
#include <array>
#include <new>

using namespace std;

//----------------------------------------------------------------------
// Static and dynamic assertion and verification macros and functions

#include "insist.h"  // valid for C and C++ both
#include "utility.h" // valid for C and C++ both

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
