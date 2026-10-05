#pragma once

#include <string_view>

namespace simpleds::core {

// Value of the cookie `name` in a Cookie request header ("a=1; b=2"), or an
// empty view if it is not present.
std::string_view findCookie(std::string_view header, std::string_view name);

}  // namespace simpleds::core
