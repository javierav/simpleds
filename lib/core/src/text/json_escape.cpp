#include "text/json_escape.h"

#include <cstdio>

namespace simpleds::core {

std::string escapeJson(std::string_view text) {
  std::string escaped;
  escaped.reserve(text.size());
  for (const char c : text) {
    switch (c) {
      case '"': escaped += "\\\""; break;
      case '\\': escaped += "\\\\"; break;
      case '\n': escaped += "\\n"; break;
      case '\r': escaped += "\\r"; break;
      case '\t': escaped += "\\t"; break;
      default:
        if (static_cast<unsigned char>(c) < 0x20) {
          char code[7];
          std::snprintf(code, sizeof(code), "\\u%04x", static_cast<unsigned>(c));
          escaped += code;
        } else {
          escaped += c;
        }
    }
  }
  return escaped;
}

}  // namespace simpleds::core
