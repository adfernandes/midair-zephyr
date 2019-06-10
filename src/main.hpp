#pragma once

//----------------------------------------------------------------------
// Core include files

#include <zephyr.h>

#include <logging/log.h>
#include <logging/log_ctrl.h>

#include <device.h>
#include <counter.h>
#include <gpio.h>
#include <spi.h>
#include <i2c.h>
#include <adc.h>
#include <pwm.h>

#define sys_panic() { log_panic(); k_panic(); }

//----------------------------------------------------------------------
// Nordic specific drivers and HAL subsystems

#include <nrfx.h>
#include <nrfx_glue.h>

#include <nrfx_ppi.h>
#include <nrfx_gpiote.h>
#include <nrfx_saadc.h>

#include <nrfx/hal/nrf_ficr.h>
#include <nrfx/hal/nrf_saadc.h>
#include <nrfx/hal/nrf_radio.h>

//----------------------------------------------------------------------
// Standard C and C++ headers

#include <stdint.h>
#include <math.h>

#include <algorithm>
#include <type_traits>

//----------------------------------------------------------------------
// Static assertions, for C and C++ both

#ifdef __cplusplus
    #ifndef _Static_assert
        #define _Static_assert static_assert
    #endif // _Static_assert
#endif // __cplusplus

#define STATIC_ASSERT(is_true) _Static_assert((is_true), #is_true )

//----------------------------------------------------------------------
// A C++14 safe way of casting an 'enum class' to it's underlying type,
// see https://stackoverflow.com/a/33083231/1229371 for details

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

//----------------------------------------------------------------------
// A helper function for something that we do a lot, note that
// STRINGIFY is defined in 'zephyr/include/toolchain/common.h'

#define insist(function)  {        \
    const int failed = (function);         \
    if (unlikely(failed)) {                \
        LOG_ERR("failed '" STRINGIFY(function) "' at " __FILE__  ":" STRINGIFY(__LINE__)); \
        sys_panic();                       \
    }                                      \
}

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
#endif

//----------------------------------------------------------------------
