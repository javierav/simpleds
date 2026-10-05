#include "auth/cookie.h"

namespace simpleds::core {

std::string_view findCookie(std::string_view header, std::string_view name) {
  while (!header.empty()) {
    const std::size_t end = header.find(';');
    std::string_view pair = header.substr(0, end);
    header = end == std::string_view::npos ? std::string_view() : header.substr(end + 1);

    while (!pair.empty() && pair.front() == ' ') pair.remove_prefix(1);
    const std::size_t eq = pair.find('=');
    if (eq != std::string_view::npos && pair.substr(0, eq) == name) return pair.substr(eq + 1);
  }
  return {};
}

}  // namespace simpleds::core
