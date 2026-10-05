#include "services/provisioning/access_point.h"

#include <WiFi.h>

#include <cstdio>

#include "config/network_defaults.h"

namespace simpleds::services {

bool AccessPoint::begin() {
  // AP+STA so the station interface can scan nearby networks.
  WiFi.mode(WIFI_AP_STA);

  uint8_t mac[6];
  WiFi.macAddress(mac);
  char suffix[5];
  std::snprintf(suffix, sizeof(suffix), "%02X%02X", mac[4], mac[5]);
  ssid_ = std::string(config::kAccessPointSsidPrefix) + suffix;

  if (!WiFi.softAP(ssid_.c_str())) {
    log_e("Failed to start access point '%s'", ssid_.c_str());
    return false;
  }
  log_i("Access point '%s' started, portal at http://%s", ssid_.c_str(),
        WiFi.softAPIP().toString().c_str());
  return true;
}

}  // namespace simpleds::services
