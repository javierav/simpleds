#pragma once

#include <cstdint>

namespace simpleds::config {

// BOOT button on ESP32 dev boards (active low, external pull-up).
inline constexpr uint8_t kResetButtonPin = 0;

}  // namespace simpleds::config
