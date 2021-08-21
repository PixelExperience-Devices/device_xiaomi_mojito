#
# Copyright (C) 2021 The PixelExperience Project
#
# SPDX-License-Identifier: Apache-2.0
#

DEVICE_PATH := device/xiaomi/mojito

# Audio
TARGET_PROVIDES_AUDIO_EXTNS := true

# Display
TARGET_SCREEN_DENSITY := 440

# Kernel
TARGET_KERNEL_CONFIG := mojito_defconfig
TARGET_KERNEL_SOURCE := kernel/xiaomi/sm6150

# Properties
TARGET_VENDOR_PROP += $(DEVICE_PATH)/vendor.prop

# Inherit from sm6150-common
include device/xiaomi/sm6150-common/BoardConfigCommon.mk

# Inherit from the proprietary version
include vendor/xiaomi/mojito/BoardConfigVendor.mk
