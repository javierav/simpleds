#pragma once

#include "settings/mqtt_settings.h"

namespace simpleds::services {

// Persists the MQTT settings in NVS. Missing or invalid values fall back to
// the factory defaults in config/mqtt_defaults.h (MQTT disabled).
class MqttSettingsStore {
 public:
  core::MqttSettings load();
  bool save(const core::MqttSettings& settings);
};

}  // namespace simpleds::services
