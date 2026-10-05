#include "services/panel/control_panel.h"

#include "services/panel/device_status.h"
#include "services/web/web_responses.h"

namespace simpleds::services {

namespace {
constexpr const char* kIndexPath = "/panel/index.html";
}  // namespace

void ControlPanel::begin() {
  server_.on("/", HTTP_GET, [this] { sendWebAsset(server_, kIndexPath); });
  server_.on("/api/status", HTTP_GET, [this] { handleGetStatus(); });
  server_.onNotFound([this] { handleNotFound(); });
  server_.begin();
  running_ = true;
  log_i("Control panel listening on port 80");
}

void ControlPanel::update() {
  if (running_) server_.handleClient();
}

void ControlPanel::handleGetStatus() {
  sendJson(server_, 200, deviceStatusToJson(readDeviceStatus()));
}

void ControlPanel::handleNotFound() {
  if (server_.method() == HTTP_GET && sendWebAsset(server_, server_.uri().c_str())) return;
  server_.send(404, "text/plain", "Not found");
}

}  // namespace simpleds::services
