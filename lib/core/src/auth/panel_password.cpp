#include "auth/panel_password.h"

namespace simpleds::core {

std::optional<ValidationError> validatePanelPassword(std::string_view password) {
  if (password.empty()) return std::nullopt;
  if (password.size() < kMinPanelPasswordLength) return ValidationError{"password", "too_short"};
  if (password.size() > kMaxPanelPasswordLength) return ValidationError{"password", "too_long"};
  return std::nullopt;
}

bool constantTimeEquals(std::string_view a, std::string_view b) {
  if (a.size() != b.size()) return false;  // Only the length leaks.
  unsigned char diff = 0;
  for (std::size_t i = 0; i < a.size(); ++i) {
    diff |= static_cast<unsigned char>(a[i] ^ b[i]);
  }
  return diff == 0;
}

}  // namespace simpleds::core
