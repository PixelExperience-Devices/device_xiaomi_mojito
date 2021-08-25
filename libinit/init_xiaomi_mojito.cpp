/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <libinit_dalvik_heap.h>
#include <libinit_variant.h>

#include "vendor_init.h"

static const variant_info_t mojito_info = {
    .hwc_value = "GLOBAL",
    .sku_value = "",

    .brand = "Redmi",
    .device = "mojito",
    .marketname = "Redmi Note 10",
    .model = "M2101K7AG",

    .nfc = false,
};

static const variant_info_t sunny_info = {
    .hwc_value = "GLOBAL",
    .sku_value = "",

    .brand = "Redmi",
    .device = "sunny",
    .marketname = "Redmi Note 10",
    .model = "M2101K7AG",

    .nfc = false,
};

static const std::vector<variant_info_t> variants = {
    mojito_info,
    sunny_info,
};

void vendor_load_properties() {
    search_variant(variants);
    set_dalvik_heap();
}
