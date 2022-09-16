#!/bin/bash
#
# Copyright (C) 2021 The PixelExperience Project
#
# SPDX-License-Identifier: Apache-2.0
#

function blob_fixup() {
    case "${1}" in
        vendor/etc/sensors/hals.conf)
            # Remove touchfeature from sensor HALs loading list
            sed -i "/sensors.touch.detect.so/d" "${2}"
            ;;
        vendor/lib64/camera/components/com.qti.node.watermark.so)
            ${PATCHELF} --add-needed "libpiex_shim.so" "${2}"
            ;;
    esac
}

# If we're being sourced by the common script that we called,
# stop right here. No need to go down the rabbit hole.
if [ "${BASH_SOURCE[0]}" != "${0}" ]; then
    return
fi

set -e

export DEVICE=mojito
export DEVICE_COMMON=sm6150-common
export VENDOR=xiaomi

"./../../${VENDOR}/${DEVICE_COMMON}/extract-files.sh" "$@"
