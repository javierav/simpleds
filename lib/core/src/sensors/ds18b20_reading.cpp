#include "sensors/ds18b20_reading.h"

#include <cmath>

namespace simpleds::core {

namespace {
constexpr float kMinC = -55.0f;
constexpr float kMaxC = 125.0f;
constexpr float kPowerOnResetC = 85.0f;
}  // namespace

std::optional<float> validateDs18b20Reading(float celsius) {
  if (!std::isfinite(celsius) || celsius < kMinC || celsius > kMaxC) return std::nullopt;
  if (celsius == kPowerOnResetC) return std::nullopt;
  return celsius;
}

}  // namespace simpleds::core
