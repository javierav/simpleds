#pragma once

namespace simpleds::core {

// A rejected setting: the field name and a stable machine-readable code, as
// returned by the web API (e.g. {"field": "port", "code": "out_of_range"}).
struct ValidationError {
  const char* field;
  const char* code;
};

}  // namespace simpleds::core
