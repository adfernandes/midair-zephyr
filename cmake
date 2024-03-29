#!/bin/bash
#
# Instead of having CLion call CMake on the $PATH, use this
# script instead. It will load the proper environment variables
# for building zephyr-based projects, also adjusting the $PATH.

set -eu -o pipefail

source "$(dirname "${BASH_SOURCE[0]}")/setup/environment"

exec cmake "${@}"

# done
