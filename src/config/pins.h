#pragma once

#include <cstdint>

namespace simpleds::config {

// BOOT button on ESP32 dev boards (active low, external pull-up).
inline constexpr uint8_t kResetButtonPin = 0;

// Outputs, active high, each driving an NPN transistor (see ARCHITECTURE.md).
// Chosen among the pins that stay low and quiet during boot.
inline constexpr uint8_t kSsrPin = 26;
inline constexpr uint8_t kFanPin = 27;

// DS18B20 probes, one per 1-Wire bus, each with a 4.7 kΩ pull-up to 3.3 V.
inline constexpr uint8_t kSsrProbePin = 32;
inline constexpr uint8_t kTankProbePin = 33;

}  // namespace simpleds::config
