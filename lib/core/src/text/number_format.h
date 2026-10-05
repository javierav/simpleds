#pragma once

#include <string>

namespace simpleds::core {

// Formats a value with one decimal ("54.3", "-2.0"), as published over MQTT.
std::string formatOneDecimal(float value);

}  // namespace simpleds::core
