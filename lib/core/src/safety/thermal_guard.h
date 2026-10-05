#pragma once

#include <cstdint>
#include <optional>

#include "safety/hysteresis_switch.h"

namespace simpleds::core {

enum class ThermalState {
  kStarting,   // No SSR reading yet: output off, no emergency reported.
  kActive,     // Normal operation.
  kEmergency,  // Overheat or SSR probe failure: output off, fan on.
};

enum class EmergencyReason { kNone, kOverheat, kSensorFailure };

// Protects the SSR from overheating. Enters emergency when its temperature
// reaches the trip threshold or its probe fails repeatedly, and leaves it
// only once a valid reading is at or below the reset threshold.
class ThermalGuard {
 public:
  ThermalGuard(float tripC, float resetC, uint8_t maxConsecutiveErrors);

  void setThresholds(float tripC, float resetC) { overheat_.setThresholds(tripC, resetC); }

  // Feeds a new SSR reading; nothing means the probe could not be read.
  void update(std::optional<float> temperatureC);

  ThermalState state() const { return state_; }
  EmergencyReason reason() const { return reason_; }
  bool allowsOutput() const { return state_ == ThermalState::kActive; }

 private:
  HysteresisSwitch overheat_;
  uint8_t maxConsecutiveErrors_;
  uint8_t consecutiveErrors_ = 0;
  ThermalState state_ = ThermalState::kStarting;
  EmergencyReason reason_ = EmergencyReason::kNone;
};

// Stable machine-readable codes, used by MQTT and the web API.
const char* stateCode(ThermalState state);
const char* reasonCode(EmergencyReason reason);

}  // namespace simpleds::core
