#pragma once

//----------------------------------------------------------------------
// Static assertions, for C and C++ both

#ifdef __cplusplus
#ifndef _Static_assert
        #define _Static_assert static_assert
    #endif // _Static_assert
#endif // __cplusplus

#define STATIC_ASSERT(is_true) _Static_assert((is_true), #is_true )

//----------------------------------------------------------------------
// A helper function for something that we do a lot, note that
// STRINGIFY is defined in 'zephyr/include/toolchain/common.h'

#define insist(function) do {              \
    const int failed = (function);         \
    if (unlikely(failed)) {                \
        log_panic();                       \
        LOG_ERR("failed '" STRINGIFY(function) "' at " __FILE__  ":" STRINGIFY(__LINE__)); \
        k_panic();                         \
    }                                      \
} while (false)

#define insist_true(failed) insist(!(failed))

//----------------------------------------------------------------------
