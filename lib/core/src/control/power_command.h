#pragma once

#include <optional>
#include <string_view>

namespace simpleds::core {

// Parses a power command payload: a decimal number of percent between 0 and
// 100 ("0", "42", "42.5"), optionally surrounded by whitespace. Returns
// nothing if the payload is not a valid command.
std::optional<float> parsePowerPercent(std::string_view payload);

}  // namespace simpleds::core
