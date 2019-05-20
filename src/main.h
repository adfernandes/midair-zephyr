#ifndef MAIN_H
#define MAIN_H
//----------------------------------------------------------------------

#include <zephyr.h>

#include <device.h>
#include <counter.h>
#include <gpio.h>
#include <spi.h>
#include <i2c.h>

#include <logging/log.h>

//----------------------------------------------------------------------

#ifdef __cplusplus
    #ifndef _Static_assert
        #define _Static_assert static_assert
    #endif // _Static_assert
#endif // __cplusplus

#define STATIC_ASSERT(is_true) _Static_assert((is_true), "(" #is_true ") failed")

//----------------------------------------------------------------------

#include "devices.h"

//----------------------------------------------------------------------

// These 'include' directives are NOT needed by Zephyr.
// They are only included so that Visual Studio Code's
// "vscode-cpptools" intellisense can work properly.
//
#if defined(VSCODE_CPPTOOLS_INTELLISENSE)
#include "zephyr/include/generated/autoconf.h"
#include "zephyr/soc/arm/nordic_nrf/nrf52/dts_fixup.h"
#include "zephyr/include/generated/generated_dts_board_unfixed.h"
#endif

//----------------------------------------------------------------------
#endif // MAIN_H
