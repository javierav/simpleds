#pragma once

#include <string>

namespace simpleds::services {

// Persists the control panel password in NVS. An empty password means the
// panel is not protected.
class PanelPasswordStore {
 public:
  std::string load();
  bool save(const std::string& password);
  void clear();
};

}  // namespace simpleds::services
