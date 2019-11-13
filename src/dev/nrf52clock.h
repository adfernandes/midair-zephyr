/* Copyright (c) 2017 - 2018, Nordic Semiconductor ASA
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
 * @brief This module defines Clock Abstraction Layer for a generic driver.
 *
 * Clock Abstraction Layer can be used by other modules to start and stop nRF528XX clocks.
 *
 * It is based on the "nrf52_clock.h" and "nrf52_clock.c" components of Zephyr 2.X.
 *
 */

#ifndef NRF52_CLOCK_H_
#define NRF52_CLOCK_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start High Frequency Clock.
 *
 * This function is asynchronous.
 *
 * It should request ramping up of HF clock and exit.
 *
 */
void nrf52_clock_hfclk_start(void);

/**
 * @brief Request stopping the High Frequency Clock.
 */
void nrf52_clock_hfclk_stop(void);

/**
 * @brief Check if High Frequency Clock was requested to run.
 *
 * @retval true  If High Frequency Clock was requested to run.
 * @retval false If High Frequency Clock was requested to stop.
 *
 */
bool nrf52_clock_hfclk_is_running(void);

/**
 * @brief Start Low Frequency Clock.
 *
 * This function is asynchronous.
 *
 * It should request ramping up of LF clock and exit.
 *
 */
void nrf52_clock_lfclk_start(void);

/**
 * @brief Request stopping the Low Frequency Clock.
 */
void nrf52_clock_lfclk_stop(void);

/**
 * @brief Check if Low Frequency Clock is running.
 *
 * @retval true  If Low Frequency Clock was requested to run.
 * @retval false If Low Frequency Clock was requested to stop.
 */
bool nrf52_clock_lfclk_is_running(void);

/**
 * @brief Set up the nRF528XX clock to be in the highest-power
 * and lowest-latency mode, using the highest-precision clocks.
 */
void nrf52_enable_maximum_performance(void);

/**
 *@}
 **/

#ifdef __cplusplus
}
#endif

#endif /* NRF52_CLOCK_H_ */
