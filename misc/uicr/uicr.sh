#!/bin/bash

set -eux -o pipefail

[[ -e "uicr.js" ]]

[[ -e "Dockerfile" ]]

docker build --tag "nordic/uicr:latest" .
docker run         "nordic/uicr:latest" > "../uicr.hex"
