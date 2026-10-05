#pragma once

#include <optional>

namespace simpleds::core {

// Filters a raw DS18B20 reading: rejects the "disconnected" marker (-127 °C),
// the power-on reset value (85 °C, read when a conversion did not happen) and
// anything outside the sensor range (-55 … 125 °C).
std::optional<float> validateDs18b20Reading(float celsius);

}  // namespace simpleds::core
