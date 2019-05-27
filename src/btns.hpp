#pragma once
//----------------------------------------------------------------------

#include "main.hpp"

#define RED_BTN_CONTROLLER DT_GPIO_KEYS_BUTTON0_RED_GPIO_CONTROLLER
#define RED_BTN_PIN        DT_GPIO_KEYS_BUTTON0_RED_GPIO_PIN

#define GRN_BTN_CONTROLLER DT_GPIO_KEYS_BUTTON1_GREEN_GPIO_CONTROLLER
#define GRN_BTN_PIN        DT_GPIO_KEYS_BUTTON1_GREEN_GPIO_PIN

enum class btn_id { red, green };

extern atomic_t red_btn_state;
extern atomic_t grn_btn_state;

inline bool get_red_btn_state(void) {
    return bool(atomic_get(&red_btn_state));
}

inline bool get_grn_btn_state(void) {
    return bool(atomic_get(&grn_btn_state));
}

// TODO: START HERE

//----------------------------------------------------------------------
