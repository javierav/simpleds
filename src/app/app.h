#pragma once

#include "config/diverter_defaults.h"
#include "config/network_defaults.h"
#include "config/pins.h"
#include "drivers/input/reset_button.h"
#include "drivers/output/fan_output.h"
#include "drivers/output/ssr_output.h"
#include "drivers/sensors/ds18b20_probe.h"
#include "services/diverter/diverter.h"
#include "services/mqtt/mqtt_bridge.h"
#include "services/mqtt/mqtt_connection.h"
#include "services/panel/control_panel.h"
#include "services/storage/control_settings_store.h"
#include "services/storage/mqtt_settings_store.h"
#include "services/storage/panel_password_store.h"
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
  services::ControlSettingsStore controlStore_;
  services::MqttSettingsStore mqttStore_;
  services::PanelPasswordStore passwordStore_;
  services::WifiManager wifi_{wifiStore_, restart_};

  drivers::SsrOutput ssr_{config::kSsrPin, config::kMainsFrequencyHz, config::kSsrLevelLeaseMs};
  drivers::FanOutput fan_{config::kFanPin};
  drivers::Ds18b20Probe ssrProbe_{config::kSsrProbePin, config::kSsrProbeIntervalMs};
  drivers::Ds18b20Probe tankProbe_{config::kTankProbePin,
                                   config::kDefaultTelemetryIntervalS * 1000};
  services::Diverter diverter_{ssr_, fan_, ssrProbe_, tankProbe_};

  services::MqttConnection mqttConnection_;
  services::MqttBridge mqtt_{mqttConnection_, diverter_};
  services::ControlPanel panel_{passwordStore_, controlStore_, mqttStore_,
                                diverter_,      mqtt_,         restart_};
  drivers::ResetButton resetButton_{config::kResetButtonPin, config::kWifiResetHoldMs};
};

}  // namespace simpleds::app
