#pragma once

#include <optional>

#include "network/wifi_credentials.h"

namespace simpleds::services {

// Persists the WiFi credentials in NVS so they survive restarts and firmware
// updates.
class WifiCredentialsStore {
 public:
  std::optional<core::WifiCredentials> load();
  bool save(const core::WifiCredentials& credentials);
  void clear();
};

}  // namespace simpleds::services
