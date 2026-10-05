#include "services/provisioning/captive_portal.h"

#include <WiFi.h>

#include "config/network_defaults.h"
#include "services/provisioning/portal_json.h"
#include "services/web/web_responses.h"

namespace simpleds::services {

namespace {
constexpr uint16_t kDnsPort = 53;
constexpr const char* kIndexPath = "/portal/index.html";

// Connectivity checks made by Android, iOS/macOS and Windows on joining a
// network. Registering them avoids "handler not found" errors in the log.
constexpr const char* kConnectivityCheckPaths[] = {
    "/generate_204", "/gen_204",    "/hotspot-detect.html", "/library/test/success.html",
    "/connecttest.txt", "/ncsi.txt", "/redirect",           "/canonical.html",
    "/success.txt",
};
}  // namespace

CaptivePortal::CaptivePortal(WifiCredentialsStore& store, RestartScheduler& restart)
    : store_(store), restart_(restart) {}

void CaptivePortal::begin() {
  // Scanning before the AP starts is much faster, so the first page load
  // already has results.
  WiFi.mode(WIFI_STA);
  scanner_.scanBlocking();

  if (!accessPoint_.begin()) return;
  tester_.begin();

  dns_.setErrorReplyCode(DNSReplyCode::NoError);
  dns_.start(kDnsPort, "*", WiFi.softAPIP());

  server_.on("/", HTTP_GET, [this] { sendWebAsset(server_, kIndexPath); });
  server_.on("/api/networks", HTTP_GET, [this] { handleGetNetworks(); });
  server_.on("/api/wifi", HTTP_POST, [this] { handleSaveWifi(); });
  server_.on("/api/wifi/status", HTTP_GET, [this] { handleGetTestStatus(); });
  for (const char* path : kConnectivityCheckPaths) {
    server_.on(path, [this] { redirectToPortal(); });
  }
  server_.onNotFound([this] { handleNotFound(); });
  server_.begin();
  running_ = true;
}

void CaptivePortal::update() {
  if (!running_) return;
  dns_.processNextRequest();
  server_.handleClient();
  scanner_.update();
  updateConnectionTest();
}

void CaptivePortal::updateConnectionTest() {
  // The radio cannot scan and connect at the same time.
  if (pendingTest_ && !scanner_.isScanning()) {
    testResultHandled_ = false;
    storageFailed_ = false;
    tester_.start(*pendingTest_);
    pendingTest_.reset();
  }

  tester_.update();
  if (tester_.state() != ConnectionTester::State::kSucceeded || testResultHandled_) return;
  testResultHandled_ = true;

  if (!store_.save(tester_.credentials())) {
    log_e("Failed to store WiFi credentials");
    storageFailed_ = true;
    return;
  }
  log_i("WiFi network '%s' configured", tester_.credentials().ssid.c_str());
  restart_.schedule(config::kRestartAfterProvisioningMs);
}

void CaptivePortal::handleGetNetworks() {
  const bool testing = pendingTest_ || tester_.state() == ConnectionTester::State::kTesting;
  if (!testing) scanner_.requestScan();
  sendJson(server_, 200, networksToJson(scanner_.networks(), scanner_.isScanning()));
}

void CaptivePortal::handleSaveWifi() {
  const bool busy = pendingTest_ || tester_.state() == ConnectionTester::State::kTesting ||
                    (tester_.state() == ConnectionTester::State::kSucceeded && !storageFailed_);
  if (busy) {
    sendJson(server_, 409, errorToJson("busy"));
    return;
  }

  const core::WifiCredentials credentials{server_.arg("ssid").c_str(),
                                          server_.arg("password").c_str()};
  const auto error = core::validateCredentials(credentials);
  if (error != core::CredentialsError::kNone) {
    sendJson(server_, 400, errorToJson(core::errorCode(error)));
    return;
  }

  pendingTest_ = credentials;
  sendJson(server_, 202, testStatusToJson("testing", nullptr));
}

void CaptivePortal::handleGetTestStatus() {
  if (pendingTest_) {
    sendJson(server_, 200, testStatusToJson("testing", nullptr));
    return;
  }
  switch (tester_.state()) {
    case ConnectionTester::State::kIdle:
      sendJson(server_, 200, testStatusToJson("idle", nullptr));
      break;
    case ConnectionTester::State::kTesting:
      sendJson(server_, 200, testStatusToJson("testing", nullptr));
      break;
    case ConnectionTester::State::kSucceeded:
      sendJson(server_, 200,
               storageFailed_
                   ? testStatusToJson("failed", "storage")
                   : testSuccessToJson(tester_.ip(), std::string(config::kHostname) + ".local"));
      break;
    case ConnectionTester::State::kFailed:
      sendJson(server_, 200, testStatusToJson("failed", core::failureCode(tester_.failure())));
      break;
  }
}

void CaptivePortal::handleNotFound() {
  if (server_.method() == HTTP_GET && sendWebAsset(server_, server_.uri().c_str())) return;
  redirectToPortal();
}

void CaptivePortal::redirectToPortal() {
  const String url = "http://" + WiFi.softAPIP().toString() + "/";
  server_.sendHeader("Location", url, true);
  server_.send(302, "text/plain", url);
}

}  // namespace simpleds::services
