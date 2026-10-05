#pragma once

#include <WebServer.h>

#include "services/diverter/diverter.h"
#include "services/mqtt/mqtt_bridge.h"
#include "services/panel/panel_auth.h"
#include "services/storage/control_settings_store.h"
#include "services/storage/mqtt_settings_store.h"
#include "services/storage/panel_password_store.h"
#include "services/system/restart_scheduler.h"

namespace simpleds::services {

// Web control panel (web/panel/) served on the local network in station mode:
// device status and every setting of the diverter.
class ControlPanel {
 public:
  ControlPanel(PanelPasswordStore& passwordStore, ControlSettingsStore& controlStore,
               MqttSettingsStore& mqttStore, Diverter& diverter, MqttBridge& mqtt,
               RestartScheduler& restart);

  void begin();
  void update();

 private:
  using Handler = void (ControlPanel::*)();

  // Registers a route that requires a session when a password is set.
  void onProtected(const char* path, HTTPMethod method, Handler handler);

  void handleIndex();
  void handleGetStatus();
  void handleGetSettings();
  void handleSaveControl();
  void handleSaveMqtt();
  void handleSavePassword();
  void handleNotFound();

  WebServer server_{80};
  PanelAuth auth_;
  ControlSettingsStore& controlStore_;
  MqttSettingsStore& mqttStore_;
  Diverter& diverter_;
  MqttBridge& mqtt_;
  RestartScheduler& restart_;
  bool running_ = false;
};

}  // namespace simpleds::services
