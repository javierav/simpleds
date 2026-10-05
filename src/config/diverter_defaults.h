#pragma once

#include <cstdint>

namespace simpleds::config {

// Mains frequency: the SSR power is modulated in whole cycles of this period.
inline constexpr uint32_t kMainsFrequencyHz = 50;

// The SSR output switches itself off if the main loop stops refreshing it for
// this long (e.g. the firmware hangs while the load is on).
inline constexpr uint32_t kSsrLevelLeaseMs = 2000;

// SSR temperature is sampled at a fixed, fast rate for the overheat protection.
inline constexpr uint32_t kSsrProbeIntervalMs = 2000;
// Consecutive failed SSR readings that trigger the emergency mode.
inline constexpr uint8_t kSsrProbeMaxErrors = 3;

// Factory values of the settings adjustable from the panel.
inline constexpr float kDefaultSsrTripC = 70.0f;
inline constexpr float kDefaultSsrResetC = 55.0f;
inline constexpr float kDefaultFanOnC = 45.0f;
inline constexpr float kDefaultFanOffC = 40.0f;
inline constexpr uint32_t kDefaultTelemetryIntervalS = 30;
inline constexpr uint32_t kDefaultCommandTimeoutS = 60;

}  // namespace simpleds::config
