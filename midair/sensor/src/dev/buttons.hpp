#pragma once

//----------------------------------------------------------------------

#include "main.hpp"
#include "devices.hpp"

enum class btn_pin : uint32_t {
    red   = RED_BTN_PIN,
    grn   = GRN_BTN_PIN,
};

extern atomic_t red_btn_state;
extern atomic_t grn_btn_state;

static inline auto get_red_btn_state(void) -> bool {
    return bool(atomic_get(&red_btn_state));
}

static inline auto get_grn_btn_state(void) -> bool {
    return bool(atomic_get(&grn_btn_state));
}

static inline auto get_btn_state(const btn_pin pin) -> bool {
    switch (pin) {
        case btn_pin::red: return get_red_btn_state();
        case btn_pin::grn: return get_grn_btn_state();
        default:           return false;
    }
}

void configure_buttons(void);

//----------------------------------------------------------------------
