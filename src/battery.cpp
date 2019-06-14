#include "battery.hpp"

LOG_MODULE_DECLARE(midair, LOG_LEVEL_DBG);

//----------------------------------------------------------------------

// The SAADC effective number of bits (ENOB) without oversampling is nine.
// So higher resolutions - usually 10, 12, or 14 - require oversampling.
//
static const u8_t bits_resolution = 14;

// // voltage = factor * sample_reading
//
static const float factor = 3.0f / float(1 << (bits_resolution - 1));

// For more information, see the blog post at
// https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/measuring-lithium-battery-voltage-with-nrf52
// since this is similar to what we have done.

//----------------------------------------------------------------------

// We technically do not need a buffer here, but this kind of setup is
// useful if, for some reason, we some day want to do multiple readings.

static const size_t n_samples = 1;
static s16_t sample_buffer[n_samples] = { 0 };

//----------------------------------------------------------------------

extern "C" enum adc_action adc_callback(
				struct device *dev,
				const struct adc_sequence *sequence,
				u16_t sampling_index) {

    char voltage[32];
    gcvtf(factor * float(sample_buffer[0]), 9, voltage);
    LOG_DBG("saadc: (%d/%u)*6 V = %s V", sample_buffer[0], (1 << bits_resolution), log_strdup(voltage));
    return ADC_ACTION_CONTINUE;

}

//----------------------------------------------------------------------

static const struct adc_channel_cfg channel_cfg = {
	.gain             = ADC_GAIN_1_5,
	.reference        = ADC_REF_INTERNAL,
	.acquisition_time = ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 3),
	.channel_id       = to_underlying(BATTERY_ADC_INPUT),
	.input_positive   = BATTERY_ADC_INPUT,
    .input_negative   = NRF_SAADC_INPUT_DISABLED
};

static const struct adc_sequence_options options = {
    .interval_us     = 0,
    .callback        = adc_callback,
    .extra_samplings = n_samples - 1,
};

static const struct adc_sequence sequence = {
    .options      = &options,
    .channels     = BIT(channel_cfg.channel_id),
    .buffer       = sample_buffer,
    .buffer_size  = sizeof(sample_buffer),
    .resolution   = bits_resolution,
    .oversampling = 5,    // the actual factor is 2^.oversampling
    .calibrate    = true, // temperature compensation calibration
};

//----------------------------------------------------------------------

extern "C" void battery_work_handler(struct k_work *work)
{
    insist(adc_read(dev.saadc0, &sequence));
}

K_WORK_DEFINE(battery_work, battery_work_handler);

extern "C" void battery_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&battery_work);
}

K_TIMER_DEFINE(battery_timer, battery_timer_handler, NULL);

//----------------------------------------------------------------------

void configure_battery(void) {

    insist(adc_channel_setup(dev.saadc0, &channel_cfg));

    LOG_DBG("battery voltage sensor configured");

    k_timer_start(&battery_timer, K_SECONDS(1), K_SECONDS(60));

    LOG_DBG("battery timer work queue started");

}

//----------------------------------------------------------------------
