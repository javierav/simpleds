#pragma once

#include <DNSServer.h>
#include <WebServer.h>

#include "services/provisioning/access_point.h"
#include "services/provisioning/network_scanner.h"
#include "services/storage/wifi_credentials_store.h"
#include "services/system/restart_scheduler.h"

namespace simpleds::services {

// Open access point plus a web form to configure the WiFi network. Every DNS
// query resolves to the device, so phones open the form automatically.
class CaptivePortal {
 public:
  CaptivePortal(WifiCredentialsStore& store, RestartScheduler& restart);

  void begin();
  void update();

 private:
  void handleRoot();
  void handleSave();
  void redirectToPortal();
  void sendHtml(int status, const std::string& html);

  WifiCredentialsStore& store_;
  RestartScheduler& restart_;
  AccessPoint accessPoint_;
  NetworkScanner scanner_;
  DNSServer dns_;
  WebServer server_{80};
  bool running_ = false;
};

}  // namespace simpleds::services
