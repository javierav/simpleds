#include "services/wifi/wifi_manager.h"

#include <WiFi.h>

#include "config/network_defaults.h"

namespace simpleds::services {

WifiManager::WifiManager(WifiCredentialsStore& store, RestartScheduler& restart)
    : store_(store), restart_(restart), portal_(store, restart) {}

void WifiManager::begin() {
  // Credentials live only in our own store; keep the core from saving a copy.
  WiFi.persistent(false);

  const auto credentials = store_.load();
  if (credentials) {
    mode_ = Mode::kStation;
    station_.begin(*credentials);
  } else {
    log_i("No WiFi network configured");
    mode_ = Mode::kProvisioning;
    portal_.begin();
  }
}

void WifiManager::update() {
  if (mode_ == Mode::kStation) {
    station_.update();
    if (station_.isConnected()) mdns_.begin(config::kHostname);
  } else {
    portal_.update();
  }
}

void WifiManager::forgetNetwork() {
  log_w("Forgetting WiFi network");
  store_.clear();
  restart_.schedule(config::kRestartDelayMs);
}

}  // namespace simpleds::services
