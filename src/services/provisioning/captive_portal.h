#pragma once

#include <DNSServer.h>
#include <WebServer.h>

#include <optional>

#include "services/provisioning/access_point.h"
#include "services/provisioning/connection_tester.h"
#include "services/provisioning/network_scanner.h"
#include "services/storage/wifi_credentials_store.h"
#include "services/system/restart_scheduler.h"

namespace simpleds::services {

// Open access point plus the web app in web/portal/ to configure the WiFi
// network. Every DNS query resolves to the device, so phones open it
// automatically. Credentials are stored only after a successful test
// connection.
class CaptivePortal {
 public:
  CaptivePortal(WifiCredentialsStore& store, RestartScheduler& restart);

  void begin();
  void update();

 private:
  void handleGetNetworks();
  void handleSaveWifi();
  void handleGetTestStatus();
  void updateConnectionTest();
  void handleNotFound();
  void redirectToPortal();

  WifiCredentialsStore& store_;
  RestartScheduler& restart_;
  AccessPoint accessPoint_;
  NetworkScanner scanner_;
  ConnectionTester tester_;
  // Credentials waiting for the current network scan to finish.
  std::optional<core::WifiCredentials> pendingTest_;
  bool testResultHandled_ = false;
  bool storageFailed_ = false;
  DNSServer dns_;
  WebServer server_{80};
  bool running_ = false;
};

}  // namespace simpleds::services
