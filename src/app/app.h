#pragma once

#include "config/network_defaults.h"
#include "config/pins.h"
#include "drivers/input/reset_button.h"
#include "services/storage/wifi_credentials_store.h"
#include "services/system/restart_scheduler.h"
#include "services/wifi/wifi_manager.h"

namespace simpleds::app {

// Composition root: builds the modules, wires their dependencies and drives
// their lifecycle. It must not contain domain logic.
class App {
 public:
  void begin();
  void update();

 private:
  services::RestartScheduler restart_;
  services::WifiCredentialsStore wifiStore_;
  services::WifiManager wifi_{wifiStore_, restart_};
  drivers::ResetButton resetButton_{config::kResetButtonPin, config::kWifiResetHoldMs};
};

}  // namespace simpleds::app
