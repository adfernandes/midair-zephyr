/* Copyright (c) 2019, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this
 *      list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *   3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * @file
 *   This file implements the nRF52 HF Clock abstraction with the Zephyr API.
 *
 * It is based on the "nrf52_clock.h" and "nrf52_clock.c" components of Zephyr 2.X.
 *
 * This implementation uses the Zephyr API for clock management.
 *
 */

#include <zephyr.h>

#include <stddef.h>

#include <compiler_abstraction.h>

#include <drivers/clock_control/nrf_clock_control.h>
#include <drivers/clock_control.h>

#include <logging/log.h>
#include <logging/log_ctrl.h>

#include <power.h>

#include "insist.h"

#include "nrf52clock.h"

LOG_MODULE_DECLARE(midair, LOG_LEVEL_DBG);

static bool hfclk_is_running;
static bool lfclk_is_running;

void nrf52_clock_hfclk_start(void) {

    struct device *clk_m16 = device_get_binding(DT_INST_0_NORDIC_NRF_CLOCK_LABEL "_16M");
    __ASSERT_NO_MSG(clk_m16 != NULL);

    clock_control_on(clk_m16, (void *)1); // Blocking call, but clock still may not have started fully on return

    hfclk_is_running = true;

}

void nrf52_clock_hfclk_stop(void) {

    struct device *clk_m16 = device_get_binding(DT_INST_0_NORDIC_NRF_CLOCK_LABEL "_16M");
    __ASSERT_NO_MSG(clk_m16 != NULL);

    hfclk_is_running = false;

    clock_control_off(clk_m16, NULL);

}

bool nrf52_clock_hfclk_is_running(void)
{
    return hfclk_is_running;
}

void nrf52_clock_lfclk_start(void) {

    struct device *clk_k32 = device_get_binding(DT_INST_0_NORDIC_NRF_CLOCK_LABEL "_32K");
    __ASSERT_NO_MSG(clk_k32 != NULL);

    clock_control_on(clk_k32, (void *)CLOCK_CONTROL_NRF_K32SRC);

    lfclk_is_running = true;

}

void nrf52_clock_lfclk_stop(void) {

    struct device *clk_k32 = device_get_binding(DT_INST_0_NORDIC_NRF_CLOCK_LABEL "_32K");
    __ASSERT_NO_MSG(clk_k32 != NULL);

    lfclk_is_running = false;

    clock_control_off(clk_k32, NULL);

}

bool nrf52_clock_lfclk_is_running(void)
{
    return lfclk_is_running;
}

void nrf52_enable_maximum_performance(void) {

    // See
    //  - https://devzone.nordicsemi.com/f/nordic-q-a/48275/raw-radio-access-in-mcuboot-which-is-based-on-zephyr-causes-strang-behavior/191403#191403

    // We disable sleep modes since we want both the lowest-possible latency and
    // not to hit the nRF52832 PAN 109 "DMA access transfers might be corrupted"

    nrf52_clock_hfclk_start(); // this will lock the device to "on" mode
    nrf52_clock_lfclk_start(); // this will lock the device to "on" mode

    // Now wait until the HFXO and LFXO have both started
    //
    // According to the nRF52832 data sheet:
    //  - the typical HFXO startup time is 360 us
    //  - the typical LFXO startup time is 600 us

    k_busy_wait(1285); //

    // Verify that everything is running as we expected

    insist_true(nrf_clock_lf_is_running(NRF_CLOCK));
    insist_true(nrf_clock_hf_is_running(NRF_CLOCK, NRF_CLOCK_HFCLK_HIGH_ACCURACY));

    const nrf_clock_lfclk_t nrf_clock_lfclk = nrf_clock_lf_src_get(NRF_CLOCK);
    insist_true(nrf_clock_lfclk == NRF_CLOCK_LFCLK_Xtal || nrf_clock_lfclk == NRF_CLOCK_LFCLK_Synth);

    // Now ensure that the kernel will never go into a "sleep" power state, and set the power mode
    // to the high-performance "constant latency" mode (which is probably moot since we are disabling
    // any sort of power-down or sleep mode that turns off the HFXO or LFXO clocks)

    NRF_POWER->TASKS_CONSTLAT = 1;
    sys_pm_force_power_state(SYS_POWER_STATE_ACTIVE);

}
