#pragma once

#include <string>

namespace simpleds::services {

// Open WiFi access point used while the device has no network configured.
class AccessPoint {
 public:
  bool begin();
  const std::string& ssid() const { return ssid_; }

 private:
  std::string ssid_;
};

}  // namespace simpleds::services
