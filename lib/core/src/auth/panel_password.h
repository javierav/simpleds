#pragma once

#include <cstddef>
#include <optional>
#include <string_view>

#include "settings/validation_error.h"

namespace simpleds::core {

inline constexpr std::size_t kMinPanelPasswordLength = 8;
inline constexpr std::size_t kMaxPanelPasswordLength = 64;

// An empty password is valid: it removes the protection of the panel.
std::optional<ValidationError> validatePanelPassword(std::string_view password);

// Compares two strings in a time that does not depend on where they differ
// (only on their length).
bool constantTimeEquals(std::string_view a, std::string_view b);

}  // namespace simpleds::core
