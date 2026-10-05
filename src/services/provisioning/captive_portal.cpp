#include "services/provisioning/captive_portal.h"

#include <WiFi.h>

#include "config/network_defaults.h"
#include "services/provisioning/portal_html.h"

namespace simpleds::services {

namespace {
constexpr uint16_t kDnsPort = 53;
constexpr const char* kHtmlContentType = "text/html; charset=utf-8";

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
  // Scanning before the AP starts is much faster, so the form is populated
  // from the first page load.
  WiFi.mode(WIFI_STA);
  scanner_.scanBlocking();

  if (!accessPoint_.begin()) return;

  dns_.setErrorReplyCode(DNSReplyCode::NoError);
  dns_.start(kDnsPort, "*", WiFi.softAPIP());

  server_.on("/", HTTP_GET, [this] { handleRoot(); });
  server_.on("/save", HTTP_POST, [this] { handleSave(); });
  for (const char* path : kConnectivityCheckPaths) {
    server_.on(path, [this] { redirectToPortal(); });
  }
  server_.onNotFound([this] { redirectToPortal(); });
  server_.begin();
  running_ = true;
}

void CaptivePortal::update() {
  if (!running_) return;
  dns_.processNextRequest();
  server_.handleClient();
  scanner_.update();
}

void CaptivePortal::handleRoot() {
  scanner_.requestScan();
  sendHtml(200, renderConfigPage(scanner_.networks(), "", ""));
}

void CaptivePortal::handleSave() {
  std::string ssid = server_.arg(kSsidField).c_str();
  if (ssid.empty()) ssid = server_.arg(kOtherSsidField).c_str();
  const core::WifiCredentials credentials{ssid, server_.arg(kPasswordField).c_str()};

  const auto error = core::validateCredentials(credentials);
  if (error != core::CredentialsError::kNone) {
    sendHtml(400, renderConfigPage(scanner_.networks(), credentials.ssid, describeError(error)));
    return;
  }

  if (!store_.save(credentials)) {
    log_e("Failed to store WiFi credentials");
    sendHtml(500, renderErrorPage("No se ha podido guardar la configuración. Inténtalo de nuevo."));
    return;
  }

  log_i("WiFi network '%s' configured", credentials.ssid.c_str());
  sendHtml(200, renderSavedPage(credentials.ssid));
  restart_.schedule(config::kRestartDelayMs);
}

void CaptivePortal::redirectToPortal() {
  const String url = "http://" + WiFi.softAPIP().toString() + "/";
  server_.sendHeader("Location", url, true);
  server_.send(302, "text/plain", url);
}

void CaptivePortal::sendHtml(int status, const std::string& html) {
  server_.send(status, kHtmlContentType, html.c_str());
}

}  // namespace simpleds::services
