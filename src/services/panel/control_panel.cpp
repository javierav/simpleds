#include "services/panel/control_panel.h"

#include "auth/panel_password.h"
#include "config/network_defaults.h"
#include "services/panel/device_status.h"
#include "services/panel/settings_form.h"
#include "services/panel/settings_json.h"
#include "services/web/web_responses.h"

namespace simpleds::services {

namespace {
constexpr const char* kIndexPath = "/panel/index.html";
constexpr const char* kLoginPath = "/panel/login.html";
}  // namespace

ControlPanel::ControlPanel(PanelPasswordStore& passwordStore, ControlSettingsStore& controlStore,
                           MqttSettingsStore& mqttStore, Diverter& diverter, MqttBridge& mqtt,
                           RestartScheduler& restart)
    : auth_(server_, passwordStore),
      controlStore_(controlStore),
      mqttStore_(mqttStore),
      diverter_(diverter),
      mqtt_(mqtt),
      restart_(restart) {}

void ControlPanel::begin() {
  auth_.begin();
  server_.on("/", HTTP_GET, [this] { handleIndex(); });
  server_.on("/api/login", HTTP_POST, [this] { auth_.handleLogin(); });
  server_.on("/api/logout", HTTP_POST, [this] { auth_.handleLogout(); });
  onProtected("/api/status", HTTP_GET, &ControlPanel::handleGetStatus);
  onProtected("/api/settings", HTTP_GET, &ControlPanel::handleGetSettings);
  onProtected("/api/settings/control", HTTP_POST, &ControlPanel::handleSaveControl);
  onProtected("/api/settings/mqtt", HTTP_POST, &ControlPanel::handleSaveMqtt);
  onProtected("/api/settings/password", HTTP_POST, &ControlPanel::handleSavePassword);
  // Static files hold no data and stay public, so the login page can load.
  server_.onNotFound([this] { handleNotFound(); });
  server_.begin();
  running_ = true;
  log_i("Control panel listening on port 80");
}

void ControlPanel::update() {
  if (running_) server_.handleClient();
}

void ControlPanel::onProtected(const char* path, HTTPMethod method, Handler handler) {
  server_.on(path, method, [this, handler] {
    if (!auth_.isAuthorized()) {
      sendJson(server_, 401, "{\"error\":\"unauthorized\"}");
      return;
    }
    (this->*handler)();
  });
}

void ControlPanel::handleIndex() {
  if (auth_.isAuthorized()) {
    sendWebAsset(server_, kIndexPath);
    return;
  }
  server_.sendHeader("Location", kLoginPath);
  server_.send(302);
}

void ControlPanel::handleGetStatus() {
  sendJson(server_, 200,
           "{\"device\":" + deviceStatusToJson(readDeviceStatus()) +
               ",\"diverter\":" + diverterStatusToJson(diverter_.status()) + ",\"mqtt\":\"" +
               bridgeStateCode(mqtt_.state()) + "\"}");
}

void ControlPanel::handleGetSettings() {
  sendJson(server_, 200,
           "{\"control\":" + controlSettingsToJson(controlStore_.load()) +
               ",\"mqtt\":" + mqttSettingsToJson(mqttStore_.load()) +
               ",\"panel\":{\"hasPassword\":" + (auth_.isProtected() ? "true" : "false") + "}}");
}

void ControlPanel::handleSaveControl() {
  const core::ControlSettings settings = controlSettingsFromForm(server_);
  if (const auto error = core::validateControlSettings(settings)) {
    sendJson(server_, 400, validationErrorToJson(*error));
    return;
  }
  if (!controlStore_.save(settings)) {
    sendJson(server_, 500, "{\"error\":\"storage\"}");
    return;
  }
  diverter_.applySettings(settings);
  log_i("Control settings updated");
  sendJson(server_, 200, controlSettingsToJson(settings));
}

void ControlPanel::handleSaveMqtt() {
  const core::MqttSettings settings = mqttSettingsFromForm(server_, mqttStore_.load());
  if (const auto error = core::validateMqttSettings(settings)) {
    sendJson(server_, 400, validationErrorToJson(*error));
    return;
  }
  if (!mqttStore_.save(settings)) {
    sendJson(server_, 500, "{\"error\":\"storage\"}");
    return;
  }
  // The MQTT client is configured once at boot: restart to apply the changes.
  log_i("MQTT settings updated");
  restart_.schedule(config::kRestartDelayMs);
  sendJson(server_, 200, "{\"restarting\":true}");
}

void ControlPanel::handleSavePassword() {
  const std::string password = server_.arg("password").c_str();
  if (const auto error = core::validatePanelPassword(password)) {
    sendJson(server_, 400, validationErrorToJson(*error));
    return;
  }
  if (!auth_.changePassword(password)) {
    sendJson(server_, 500, "{\"error\":\"storage\"}");
    return;
  }
  sendJson(server_, 200,
           std::string("{\"hasPassword\":") + (password.empty() ? "false" : "true") + "}");
}

void ControlPanel::handleNotFound() {
  if (server_.method() == HTTP_GET && sendWebAsset(server_, server_.uri().c_str())) return;
  server_.send(404, "text/plain", "Not found");
}

}  // namespace simpleds::services
