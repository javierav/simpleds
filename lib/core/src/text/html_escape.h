#pragma once

#include <string>
#include <string_view>

namespace simpleds::core {

// Escapes text so it can be safely embedded in HTML content or attributes.
std::string escapeHtml(std::string_view text);

}  // namespace simpleds::core
