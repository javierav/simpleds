#include "control/power_command.h"

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <string>

namespace simpleds::core {

namespace {
constexpr std::size_t kMaxPayloadLength = 16;

bool isSpace(char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; }
}  // namespace

std::optional<float> parsePowerPercent(std::string_view payload) {
  while (!payload.empty() && isSpace(payload.front())) payload.remove_prefix(1);
  while (!payload.empty() && isSpace(payload.back())) payload.remove_suffix(1);
  if (payload.empty() || payload.size() > kMaxPayloadLength) return std::nullopt;

  // Only plain decimal notation: no sign, exponent, hex, "inf" or "nan".
  int dots = 0;
  for (const char c : payload) {
    if (c == '.') {
      ++dots;
    } else if (!std::isdigit(static_cast<unsigned char>(c))) {
      return std::nullopt;
    }
  }
  if (dots > 1 || payload == ".") return std::nullopt;

  const std::string text(payload);
  const float percent = std::strtof(text.c_str(), nullptr);
  if (!std::isfinite(percent) || percent < 0.0f || percent > 100.0f) return std::nullopt;
  return percent;
}

}  // namespace simpleds::core
