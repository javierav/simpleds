#pragma once

#include <cstdint>

#include "network/wifi_credentials.h"

namespace simpleds::services {

// Keeps the device connected to the configured WiFi network, retrying
// periodically while the network is unreachable.
class WifiStation {
 public:
  void begin(const core::WifiCredentials& credentials);
  void update();
  bool isConnected() const;

 private:
  void connect();

  core::WifiCredentials credentials_;
  uint32_t lastAttemptMs_ = 0;
  bool connected_ = false;
};

}  // namespace simpleds::services
