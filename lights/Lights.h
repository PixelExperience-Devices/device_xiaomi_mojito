/*
 * Copyright (C) 2020 The Android Open Source Project
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

#pragma once

#include <aidl/android/hardware/light/BnLights.h>
#include <hardware/hardware.h>
#include <hardware/lights.h>
#include <map>
#include <sstream>
#include <unordered_map>

namespace aidl {
namespace android {
namespace hardware {
namespace light {

class Lights : public BnLights {
  public:
    Lights();
    ndk::ScopedAStatus setLightState(int id, const HwLightState& state) override;
    ndk::ScopedAStatus getLights(std::vector<HwLight>* types) override;

  private:
    void setLightBacklight(LightType type, const HwLightState& state);
    void setLightNotification(LightType type, const HwLightState& state);
    void applyNotificationState(const HwLightState& state);

    uint32_t max_led_brightness_;
    uint32_t max_screen_brightness_;

    std::unordered_map<LightType, std::function<void(LightType type, const HwLightState&)>> lights_{
            {LightType::ATTENTION, [this](auto&&... args) { setLightNotification(args...); }},
            {LightType::BACKLIGHT, [this](auto&&... args) { setLightBacklight(args...); }},
            {LightType::BATTERY, [this](auto&&... args) { setLightNotification(args...); }},
            {LightType::NOTIFICATIONS, [this](auto&&... args) { setLightNotification(args...); }}};

    // Keep sorted in the order of importance.
    std::array<std::pair<LightType, HwLightState>, 3> notif_states_ = {{
            {LightType::ATTENTION, {}},
            {LightType::NOTIFICATIONS, {}},
            {LightType::BATTERY, {}},
    }};
};

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl
