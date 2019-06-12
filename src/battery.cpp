#include "battery.hpp"

LOG_MODULE_DECLARE(midair, LOG_LEVEL_DBG);

//----------------------------------------------------------------------

// https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/measuring-lithium-battery-voltage-with-nrf52

static const u32_t bits_resolution = 12; // usually 10, 12, or 14

static const u32_t channel_id = to_underlying(BATTERY_ADC_INPUT);

static const size_t buffer_size = 1;
static s16_t sample_buffer[buffer_size] = { 0 };

static const struct adc_channel_cfg channel_cfg = {
	.gain             = ADC_GAIN_1_5,
	.reference        = ADC_REF_INTERNAL,
	.acquisition_time = ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 3),
	.channel_id       = channel_id,
	.input_positive   = BATTERY_ADC_INPUT,
    .input_negative   = NRF_SAADC_INPUT_DISABLED
};

extern "C" enum adc_action adc_callback(
				struct device *dev,
				const struct adc_sequence *sequence,
				u16_t sampling_index);

static const struct adc_sequence_options options = {
    .interval_us     = 0,
    .callback        = adc_callback,
    .extra_samplings = 0,
};

static const struct adc_sequence sequence = {
    .options      = &options,
    .channels     = BIT(channel_id),
    .buffer       = sample_buffer,
    .buffer_size  = sizeof(sample_buffer),
    .resolution   = bits_resolution,
    .oversampling = 3, // set to 2^n
    .calibrate    = true,
};

const float factor = 3.0f / float(1 << (bits_resolution - 1));

//----------------------------------------------------------------------

enum adc_action adc_callback(
				struct device *dev,
				const struct adc_sequence *sequence,
				u16_t sampling_index) {

    char voltage[32];
    gcvtf(factor * float(sample_buffer[0]), 10, voltage);
    LOG_DBG("saadc: %d -> %s V", sample_buffer[0], log_strdup(voltage));
    return ADC_ACTION_CONTINUE;

}

//----------------------------------------------------------------------

void configure_battery(void) {

    insist(adc_channel_setup(dev.saadc0, &channel_cfg));

    LOG_DBG("battery voltage sensor configured");

    for (u32_t i = 0; i < 4; i++) {
        insist(adc_read(dev.saadc0, &sequence));
        k_sleep(100);
    }

}

//----------------------------------------------------------------------
