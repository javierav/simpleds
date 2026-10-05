#pragma once

#include "services/provisioning/captive_portal.h"
#include "services/storage/wifi_credentials_store.h"
#include "services/system/restart_scheduler.h"
#include "services/wifi/mdns_responder.h"
#include "services/wifi/wifi_station.h"

namespace simpleds::services {

// Chooses the network mode at boot: joins the configured network or, if
// there is none, starts the provisioning portal.
class WifiManager {
 public:
  enum class Mode { kStation, kProvisioning };

  WifiManager(WifiCredentialsStore& store, RestartScheduler& restart);

  void begin();
  void update();
  Mode mode() const { return mode_; }

  // Erases the stored network and restarts into provisioning mode.
  void forgetNetwork();

 private:
  WifiCredentialsStore& store_;
  RestartScheduler& restart_;
  WifiStation station_;
  MdnsResponder mdns_;
  CaptivePortal portal_;
  Mode mode_ = Mode::kProvisioning;
};

}  // namespace simpleds::services
