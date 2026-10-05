#pragma once

#include <cstdint>
#include <optional>

#include "safety/thermal_guard.h"

namespace simpleds::services {

struct DiverterStatus {
  core::ThermalState state;
  core::EmergencyReason reason;
  float outputPercent;                   // Power level applied to the load.
  std::optional<float> commandPercent;   // Command in force, if any.
  std::optional<float> ssrTemperatureC;  // Nothing if the last reading failed.
  std::optional<float> tankTemperatureC;
  uint32_t ssrSamples;  // Readings taken so far (0: none yet).
  uint32_t tankSamples;
  bool fanOn;
};

}  // namespace simpleds::services
