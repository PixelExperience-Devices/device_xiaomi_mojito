#
# Copyright (C) 2021 The PixelExperience Project
#
# SPDX-License-Identifier: Apache-2.0
#

# API level, the device has been commercially launched on
PRODUCT_SHIPPING_API_LEVEL := 30

# Boot animation
TARGET_SCREEN_HEIGHT := 2400
TARGET_SCREEN_WIDTH := 1080

# Device uses high-density artwork where available
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xhdpi

# Dynamic partitions
PRODUCT_USE_DYNAMIC_PARTITIONS := true
PRODUCT_BUILD_SUPER_PARTITION := false

# Sensor
TARGET_ENABLE_MULTI_SENSOR := true

# Soong namespaces
PRODUCT_SOONG_NAMESPACES += \
    $(LOCAL_PATH)

# Inherit from sm6150-common
$(call inherit-product, device/xiaomi/sm6150-common/sm6150.mk)

# Inherit from vendor blobs
$(call inherit-product, vendor/xiaomi/mojito/mojito-vendor.mk)
