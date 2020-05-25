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
// User threads are not allowed to induce kernel panics, use an 'oops' instead
//
// Also, CLion does not understand https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
// so use '##__VA_ARGS__' rather than the more modern '__VA_OPT__(,) __VA_ARGS__' style

#define sys_panic(message, ...) LOG_PANIC(); LOG_ERR(message, ##__VA_ARGS__); k_oops()

//----------------------------------------------------------------------
// A helper function for something that we do a lot, note that
// STRINGIFY is defined in 'zephyr/include/toolchain/common.h'

#define insist(function) do {                                                                \
    const int failed = (function);                                                           \
    if (unlikely(failed)) {                                                                  \
        sys_panic("failed '" STRINGIFY(function) "' at " __FILE__  ":" STRINGIFY(__LINE__)); \
    }                                                                                        \
} while (false)

#define insist_that(failed) insist(!(failed))

//----------------------------------------------------------------------
