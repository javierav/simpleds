#pragma once

#include <cstdint>
#include <optional>

#include "settings/validation_error.h"

namespace simpleds::core {

// User-adjustable behaviour of the diverter (configured from the panel).
struct ControlSettings {
  float ssrTripC;               // SSR temperature that triggers the emergency mode.
  float ssrResetC;              // SSR temperature at which normal operation resumes.
  float fanOnC;                 // SSR temperature that turns the fan on…
  float fanOffC;                // …and that turns it off again.
  uint32_t telemetryIntervalS;  // Temperature sampling and publishing period.
  uint32_t commandTimeoutS;     // A power command expires after this time.
};

inline constexpr float kMinThresholdC = 20.0f;
inline constexpr float kMaxThresholdC = 100.0f;
inline constexpr uint32_t kMinTelemetryIntervalS = 5;
inline constexpr uint32_t kMaxTelemetryIntervalS = 3600;
inline constexpr uint32_t kMinCommandTimeoutS = 10;
inline constexpr uint32_t kMaxCommandTimeoutS = 3600;

std::optional<ValidationError> validateControlSettings(const ControlSettings& settings);

}  // namespace simpleds::core
