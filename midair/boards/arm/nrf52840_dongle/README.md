This board is the **same** as the `nrf52840dongle_nrf52840` board except for the following changes:
- the name `nrf52840dongle_nrf52840` was changed to `nrf52840_dongle` to avoid conflicts
- the voltage `UICR_REGOUT0_VOUT_3V0` was changed to `UICR_REGOUT0_VOUT_3V3` in `board.c`
- the `.dts` device tree was changed to include `fstab-debugger.dts` rather than `fstab-stock.dts` since we are **not** using the Nordic boot loader
- the `Kconfig` was changed so that `BOARD_HAS_NRF5_BOOTLOADER` defaults to `n`
- the `doc` directory was removed
