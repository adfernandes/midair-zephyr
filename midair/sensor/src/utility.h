#pragma once

//----------------------------------------------------------------------

extern "C" char *gcvtf(float, int, char *); // newlib: float -> string

//----------------------------------------------------------------------

#define DEVICE_GET_BINDING(port, name)   \
    (port) = device_get_binding((name)); \
    if (unlikely((port) == NULL)) {                             \
        sys_panic("device_get_binding(\"%s\") failed", (name)); \
    }

//----------------------------------------------------------------------
#ifdef __cplusplus

// A C++14 safe way of casting an 'enum class' to it's underlying type,
// see https://stackoverflow.com/a/33083231/1229371 for details

#include <type_traits>

template <typename E>
constexpr auto to_underlying(E e) noexcept;

template<typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

#endif // ! __cplusplus
//----------------------------------------------------------------------
