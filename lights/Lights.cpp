/*
 * Copyright (C) 2019 The Android Open Source Project
 * Copyright (C) 2020 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Lights.h"
#include <android-base/file.h>
#include <android-base/logging.h>

namespace {

/* clang-format off */
#define PPCAT_NX(A, B) A/B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define STRINGIFY_INNER(x) #x
#define STRINGIFY(x) STRINGIFY_INNER(x)

#define LCD(x) PPCAT(/sys/class/backlight, x)
#define LCD_ATTR(x) STRINGIFY(PPCAT(LCD(panel0-backlight), x))

#define LEDS(x) PPCAT(/sys/class/leds, x)
#define WHITE_ATTR(x) STRINGIFY(PPCAT(LEDS(white), x))
/* clang-format on */

using ::android::base::ReadFileToString;
using ::android::base::WriteStringToFile;

// Default max brightness
constexpr auto kDefaultMaxLedBrightness = 255;
constexpr auto kDefaultMaxScreenBrightness = 4095;

// Write value to path and close file.
bool WriteToFile(const std::string& path, uint32_t content) {
    return WriteStringToFile(std::to_string(content), path);
}

uint32_t RgbaToBrightness(uint32_t color) {
    // Extract brightness from AARRGGBB.
    uint32_t alpha = (color >> 24) & 0xFF;

    // Retrieve each of the RGB colors
    uint32_t red = (color >> 16) & 0xFF;
    uint32_t green = (color >> 8) & 0xFF;
    uint32_t blue = color & 0xFF;

    // Scale RGB colors if a brightness has been applied by the user
    if (alpha != 0xFF) {
        red = red * alpha / 0xFF;
        green = green * alpha / 0xFF;
        blue = blue * alpha / 0xFF;
    }

    return (77 * red + 150 * green + 29 * blue) >> 8;
}

inline uint32_t RgbaToBrightness(uint32_t color, uint32_t max_brightness) {
    return RgbaToBrightness(color) * max_brightness / 0xFF;
}

inline bool IsLit(uint32_t color) {
    return color & 0x00ffffff;
}

}  // anonymous namespace

namespace aidl {
namespace android {
namespace hardware {
namespace light {

Lights::Lights() {
    std::string buf;

    if (ReadFileToString(LCD_ATTR(max_brightness), &buf)) {
        max_screen_brightness_ = std::stoi(buf);
    } else {
        max_screen_brightness_ = kDefaultMaxScreenBrightness;
        LOG(ERROR) << "Failed to read max screen brightness, fallback to "
                   << kDefaultMaxScreenBrightness;
    }

    if (ReadFileToString(WHITE_ATTR(max_brightness), &buf)) {
        max_led_brightness_ = std::stoi(buf);
    } else {
        max_led_brightness_ = kDefaultMaxLedBrightness;
        LOG(ERROR) << "Failed to read max LED brightness, fallback to " << kDefaultMaxLedBrightness;
    }
}

ndk::ScopedAStatus Lights::setLightState(int /*id*/, const HwLightState& state) {
    for (auto&& light : lights_) {
        LightType type = light.first;
        auto it = lights_.find(type);

        if (it == lights_.end()) {
            return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
        }

        it->second(type, state);
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Lights::getLights(std::vector<HwLight>* /*lights*/) {
    return ndk::ScopedAStatus::ok();
}

void Lights::setLightBacklight(LightType /*type*/, const HwLightState& state) {
    uint32_t brightness = RgbaToBrightness(state.color, max_screen_brightness_);
    WriteToFile(LCD_ATTR(brightness), brightness);
}

void Lights::setLightNotification(LightType type, const HwLightState& state) {
    bool found = false;
    for (auto&& [cur_type, cur_state] : notif_states_) {
        if (cur_type == type) {
            cur_state = state;
        }

        // Fallback to battery light
        if (!found && (cur_type == LightType::BATTERY || IsLit(cur_state.color))) {
            found = true;
            LOG(DEBUG) << __func__ << ": type=" << toString(cur_type);
            applyNotificationState(cur_state);
        }
    }
}

void Lights::applyNotificationState(const HwLightState& state) {
    uint32_t white_brightness = RgbaToBrightness(state.color, max_led_brightness_);

    // Turn off the leds (initially)
    WriteToFile(WHITE_ATTR(breath), 0);

    if (state.flashMode == FlashMode::TIMED && state.flashOnMs > 0 && state.flashOffMs > 0) {
        LOG(DEBUG) << __func__ << ": color=" << std::hex << state.color << std::dec
                   << " onMs=" << state.flashOnMs << " offMs=" << state.flashOffMs;

        // White
        WriteToFile(WHITE_ATTR(delay_off), static_cast<uint32_t>(state.flashOffMs));
        WriteToFile(WHITE_ATTR(delay_on), static_cast<uint32_t>(state.flashOnMs));
        WriteToFile(WHITE_ATTR(breath), 1);
    } else {
        WriteToFile(WHITE_ATTR(brightness), white_brightness);
    }
}

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl
