#pragma once

#include <string>
#include <string_view>

namespace simpleds::core {

// Escapes text for use inside a JSON string literal (quotes not included).
std::string escapeJson(std::string_view text);

}  // namespace simpleds::core
