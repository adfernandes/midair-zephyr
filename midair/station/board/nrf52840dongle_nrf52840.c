/*
 * Copyright (c) 2018 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <init.h>
#include <hal/nrf_power.h>

static int board_nrf52840dongle_nrf52840_init_3v3(struct device *dev) {

    ARG_UNUSED(dev);

    /* If the nrf52840_dongle board is powered from USB (high voltage mode) the GPIO output voltage
     * is set to 1.8 volts by default and that is not enough to turn the green and blue LEDs on.
     *
     * Set the GPIO voltage to 3.3 volts if it is not already set to that voltage, specifically.
     *
     * Note that the voltage may be set to 3.0 volts by the 'board_nrf52840dongle_nrf52840_init' function
     * that is part of the standard 'nrf52840dongle_nrf52840' board support setup in the Zephyr source code.
     *
     */
    if ((nrf_power_mainregstatus_get(NRF_POWER) == NRF_POWER_MAINREGSTATUS_HIGH) &&
        ((NRF_UICR->REGOUT0 & UICR_REGOUT0_VOUT_Msk) != (UICR_REGOUT0_VOUT_3V3 << UICR_REGOUT0_VOUT_Pos))) {

        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) { /* wait */ }

        NRF_UICR->REGOUT0 =
            (NRF_UICR->REGOUT0 & ~((uint32_t)UICR_REGOUT0_VOUT_Msk)) | (UICR_REGOUT0_VOUT_3V3 << UICR_REGOUT0_VOUT_Pos);

        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) { /* wait */ }

        NVIC_SystemReset(); /* a reset is required for changes to take effect */

    }

    return 0;

}

SYS_INIT(board_nrf52840dongle_nrf52840_init_3v3, PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
