#include "main.h"

LOG_MODULE_DECLARE(midair);

struct devices dev = {  };

static atomic_t is_initialized = ATOMIC_INIT(false);;

#define DEVICE_GET_BINDING(var, label)   \
    (var) = device_get_binding((label)); \
    if (!var) {                                                \
        LOG_ERR("device_get_binding(\"%s\") failed", (label)); \
        k_panic();                                             \
    }

void devices_init(void) {

    if (atomic_get(&is_initialized)) return;

    DEVICE_GET_BINDING(dev.gpio0, DT_NORDIC_NRF_GPIO_0_LABEL);

    DEVICE_GET_BINDING(dev.red_led, RED_LED_CONTROLLER);
    DEVICE_GET_BINDING(dev.grn_led, GRN_LED_CONTROLLER);

    DEVICE_GET_BINDING(dev.red_btn, RED_BTN_CONTROLLER);
    DEVICE_GET_BINDING(dev.grn_btn, GRN_BTN_CONTROLLER);

    DEVICE_GET_BINDING(dev.saadc0, DT_NORDIC_NRF_SAADC_0_LABEL);

    DEVICE_GET_BINDING(dev.spi0, DT_SPI_0_NAME);
    DEVICE_GET_BINDING(dev.spi0cs, DT_NORDIC_NRF_SPI_0_CS_GPIOS_CONTROLLER);

    DEVICE_GET_BINDING(dev.i2c1, DT_I2C_1_NAME);

    DEVICE_GET_BINDING(dev.rtc2, DT_NORDIC_NRF_RTC_RTC_2_LABEL);

    DEVICE_GET_BINDING(dev.timer1, DT_NORDIC_NRF_TIMER_TIMER_1_LABEL);
    DEVICE_GET_BINDING(dev.timer2, DT_NORDIC_NRF_TIMER_TIMER_2_LABEL);
    DEVICE_GET_BINDING(dev.timer3, DT_NORDIC_NRF_TIMER_TIMER_3_LABEL);
    DEVICE_GET_BINDING(dev.timer4, DT_NORDIC_NRF_TIMER_TIMER_4_LABEL);

    atomic_set(&is_initialized, true);

}
