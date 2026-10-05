#include "services/wifi/wifi_station.h"

#include <WiFi.h>

#include "config/network_defaults.h"

namespace simpleds::services {

void WifiStation::begin(const core::WifiCredentials& credentials) {
  credentials_ = credentials;
  WiFi.setHostname(config::kHostname);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(false);  // Retries are handled in update().
  connect();
}

void WifiStation::update() {
  const bool connected = WiFi.status() == WL_CONNECTED;
  if (connected != connected_) {
    connected_ = connected;
    if (connected) {
      log_i("Connected to '%s', IP %s, RSSI %d dBm", credentials_.ssid.c_str(),
            WiFi.localIP().toString().c_str(), WiFi.RSSI());
    } else {
      log_w("Disconnected from '%s'", credentials_.ssid.c_str());
    }
  }

  if (!connected && millis() - lastAttemptMs_ >= config::kWifiReconnectIntervalMs) {
    connect();
  }
}

bool WifiStation::isConnected() const { return connected_; }

void WifiStation::connect() {
  log_i("Connecting to '%s'", credentials_.ssid.c_str());
  lastAttemptMs_ = millis();
  WiFi.disconnect();
  WiFi.begin(credentials_.ssid.c_str(), credentials_.password.c_str());
}

}  // namespace simpleds::services
