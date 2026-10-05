#include "safety/thermal_guard.h"

namespace simpleds::core {

ThermalGuard::ThermalGuard(float tripC, float resetC, uint8_t maxConsecutiveErrors)
    : overheat_(tripC, resetC), maxConsecutiveErrors_(maxConsecutiveErrors) {}

void ThermalGuard::update(std::optional<float> temperatureC) {
  if (!temperatureC) {
    if (consecutiveErrors_ < maxConsecutiveErrors_) ++consecutiveErrors_;
    if (consecutiveErrors_ >= maxConsecutiveErrors_) {
      state_ = ThermalState::kEmergency;
      reason_ = EmergencyReason::kSensorFailure;
      // Without readings the temperature is unknown: once the probe recovers,
      // resume only from the reset threshold, as after an overheat.
      overheat_.forceOn();
    }
    return;
  }
  consecutiveErrors_ = 0;

  if (overheat_.update(*temperatureC)) {
    state_ = ThermalState::kEmergency;
    reason_ = EmergencyReason::kOverheat;
  } else {
    state_ = ThermalState::kActive;
    reason_ = EmergencyReason::kNone;
  }
}

const char* stateCode(ThermalState state) {
  switch (state) {
    case ThermalState::kStarting: return "starting";
    case ThermalState::kActive: return "active";
    case ThermalState::kEmergency: return "emergency";
  }
  return "unknown";
}

const char* reasonCode(EmergencyReason reason) {
  switch (reason) {
    case EmergencyReason::kNone: return "none";
    case EmergencyReason::kOverheat: return "overheat";
    case EmergencyReason::kSensorFailure: return "sensor_failure";
  }
  return "unknown";
}

}  // namespace simpleds::core
