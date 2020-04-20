#!/bin/bash

set -eu -o pipefail

[[ -z ${HOME:-} ]] && export HOME=~andrew
export PATH="/opt/local/libexec/gnubin:/opt/local/bin:${PATH}"

export PROJECT_DIR="${HOME}/Projects/Nordic/midair/zephyr"

export ZEPHYR_BASE="${PROJECT_DIR}/zephyr"

export ZEPHYR_TOOLCHAIN_VARIANT="gnuarmemb" # "zephyr" or "gnuarmemb"
unset  ZEPHYR_SDK_INSTALL_DIR # "${HOME}/Developer/Toolchains/zephyr-sdk-ng"
export GNUARMEMB_TOOLCHAIN_PATH="${HOME}/Developer/Toolchains/gcc-arm-none-eabi"

export PATH="${HOME}/Developer/Nordic/Tools/Default/mergehex:${PATH}"
export PATH="${HOME}/Developer/Nordic/Tools/Default/nrfjprog:${PATH}"
export PATH="${PROJECT_DIR}/zephyr/scripts:${PATH}"

export ZEPHYR_TOOLCHAIN_VARIANT="gnuarmemb" # "zephyr" or "gnuarmemb"
unset  ZEPHYR_SDK_INSTALL_DIR # "${HOME}/Developer/Toolchains/zephyr-sdk-ng"
export GNUARMEMB_TOOLCHAIN_PATH="${HOME}/Developer/Toolchains/gcc-arm-none-eabi"

export PATH="${HOME}/Developer/Nordic/Tools/Default/nrfjprog:${PATH}"
export PATH="${HOME}/Developer/Nordic/Tools/Default/mergehex:${PATH}"

unset  ZEPHYR_MODULES                       # because we are not using west-management
export WEST_DIR="${PROJECT_DIR}/.west/west" # so ninja can use west to flash and debug

set +u
source "/opt/conda/bin/activate" zephyr
set -u

source "${PROJECT_DIR}/zephyr/zephyr-env.sh"

export BOARD="nrf52832_midair"

exec cmake "${@}"

# Done
