#include "text/number_format.h"

#include <cstdio>

namespace simpleds::core {

std::string formatOneDecimal(float value) {
  char text[16];
  std::snprintf(text, sizeof(text), "%.1f", static_cast<double>(value));
  // Avoid "-0.0" for values that round to zero.
  if (text[0] == '-' && std::string(text + 1) == "0.0") return "0.0";
  return text;
}

}  // namespace simpleds::core
