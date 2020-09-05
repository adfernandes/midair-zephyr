#include <zephyr.h>
#include <devicetree.h>

#include <drivers/adc.h>
#include <nrfx/hal/nrf_saadc.h>

#include "insist.h"
#include "utility.h"

#include "battery.hpp"

LOG_MODULE_REGISTER(battery, LOG_LEVEL_DBG);

//----------------------------------------------------------------------

static const struct device *saadc0;

static const size_t n_samples = 1;
static int16_t sample_buffer[n_samples] = { 0 };

//----------------------------------------------------------------------

#define VBATT DT_PATH(vbatt)

// The 'SAADC_CH_PSELP_PSELP_AnalogInput0' offset is required by the 'nrfx' driver
//
static const struct adc_channel_cfg channel_cfg = {
	.gain             = ADC_GAIN_1_5,
	.reference        = ADC_REF_INTERNAL,
	.acquisition_time = ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 40),
	.channel_id       = SAADC_CH_PSELP_PSELP_AnalogInput0 + DT_IO_CHANNELS_INPUT(VBATT),
	.input_positive   = SAADC_CH_PSELP_PSELP_AnalogInput0 + DT_IO_CHANNELS_INPUT(VBATT),
    .input_negative   = NRF_SAADC_INPUT_DISABLED
};

// The SAADC effective number of bits (ENOB) without oversampling is nine.
// So higher resolutions - usually 10, 12, or 14 - require oversampling.
//
static const uint8_t bits_resolution = 14;

// voltage = factor * sample_reading, for the 'channel_cfg' above
//
static const float factor = 3.0f / float(1 << (bits_resolution - 1));

static const struct adc_sequence sequence = {
    .options      = nullptr,
    .channels     = BIT(channel_cfg.channel_id),
    .buffer       = sample_buffer,
    .buffer_size  = sizeof(sample_buffer),
    .resolution   = bits_resolution,
    .oversampling = 0,    // the actual factor is 2^.oversampling
    .calibrate    = true, // temperature compensation calibration
};

//----------------------------------------------------------------------

extern "C" void battery_work_handler(struct k_work *work)
{

    insist(adc_read(saadc0, &sequence));

    char voltage[32];
    gcvtf(factor * float(sample_buffer[0]), 9, voltage);
    LOG_DBG("read (%d/%u)*6 V = %s V", sample_buffer[0], (1 << bits_resolution), log_strdup(voltage));

}

K_WORK_DEFINE(battery_work, battery_work_handler);

extern "C" void battery_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&battery_work);
}

K_TIMER_DEFINE(battery_timer, battery_timer_handler, NULL);

//----------------------------------------------------------------------

void configure_battery(void)
{

    DEVICE_GET_BINDING(saadc0, DT_LABEL(DT_NODELABEL(adc)));

    insist((2 * DT_PROP(VBATT, output_ohms)) != DT_PROP(VBATT, full_ohms));

    insist(adc_channel_setup(saadc0, &channel_cfg));

    LOG_DBG("battery voltage sensor configured");

    k_timer_start(&battery_timer, K_SECONDS(1), K_SECONDS(60));

    LOG_DBG("battery timer work queue started");

}

//----------------------------------------------------------------------
