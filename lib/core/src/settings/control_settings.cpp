#include "settings/control_settings.h"

#include <cmath>

namespace simpleds::core {

namespace {
bool isValidThreshold(float celsius) {
  return std::isfinite(celsius) && celsius >= kMinThresholdC && celsius <= kMaxThresholdC;
}
}  // namespace

std::optional<ValidationError> validateControlSettings(const ControlSettings& settings) {
  if (!isValidThreshold(settings.ssrTripC)) return ValidationError{"ssrTripC", "out_of_range"};
  if (!isValidThreshold(settings.ssrResetC)) return ValidationError{"ssrResetC", "out_of_range"};
  if (!isValidThreshold(settings.fanOnC)) return ValidationError{"fanOnC", "out_of_range"};
  if (!isValidThreshold(settings.fanOffC)) return ValidationError{"fanOffC", "out_of_range"};
  if (settings.ssrResetC >= settings.ssrTripC) {
    return ValidationError{"ssrResetC", "not_below_trip"};
  }
  if (settings.fanOffC >= settings.fanOnC) return ValidationError{"fanOffC", "not_below_on"};
  // The fan must start cooling before the emergency cuts the load.
  if (settings.fanOnC >= settings.ssrTripC) return ValidationError{"fanOnC", "not_below_trip"};
  if (settings.telemetryIntervalS < kMinTelemetryIntervalS ||
      settings.telemetryIntervalS > kMaxTelemetryIntervalS) {
    return ValidationError{"telemetryIntervalS", "out_of_range"};
  }
  if (settings.commandTimeoutS < kMinCommandTimeoutS ||
      settings.commandTimeoutS > kMaxCommandTimeoutS) {
    return ValidationError{"commandTimeoutS", "out_of_range"};
  }
  return std::nullopt;
}

}  // namespace simpleds::core
