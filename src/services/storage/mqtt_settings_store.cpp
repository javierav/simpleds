#include "services/storage/mqtt_settings_store.h"

#include <Preferences.h>

#include "config/mqtt_defaults.h"
#include "config/network_defaults.h"

namespace simpleds::services {

namespace {
constexpr const char* kNamespace = "mqtt";

core::MqttSettings defaults() {
  return {"",
          config::kDefaultMqttPort,
          "",
          "",
          config::kHostname,
          {config::kDefaultCommandTopic, config::kDefaultPowerTopic, config::kDefaultStateTopic,
           config::kDefaultSsrTemperatureTopic, config::kDefaultTankTemperatureTopic,
           config::kDefaultFanTopic}};
}

std::string getString(Preferences& prefs, const char* key, const std::string& fallback) {
  return prefs.getString(key, fallback.c_str()).c_str();
}

// Strings are written even when empty, so putString's 0 is not an error.
bool putString(Preferences& prefs, const char* key, const std::string& value) {
  return prefs.putString(key, value.c_str()) == value.size();
}
}  // namespace

core::MqttSettings MqttSettingsStore::load() {
  const core::MqttSettings factory = defaults();
  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return factory;

  core::MqttSettings s;
  s.host = getString(prefs, "host", factory.host);
  s.port = prefs.getUShort("port", factory.port);
  s.username = getString(prefs, "username", factory.username);
  s.password = getString(prefs, "password", factory.password);
  s.clientId = getString(prefs, "clientId", factory.clientId);
  s.topics.command = getString(prefs, "tCommand", factory.topics.command);
  s.topics.power = getString(prefs, "tPower", factory.topics.power);
  s.topics.state = getString(prefs, "tState", factory.topics.state);
  s.topics.ssrTemperature = getString(prefs, "tSsrTemp", factory.topics.ssrTemperature);
  s.topics.tankTemperature = getString(prefs, "tTankTemp", factory.topics.tankTemperature);
  s.topics.fan = getString(prefs, "tFan", factory.topics.fan);
  prefs.end();

  if (core::validateMqttSettings(s)) {
    log_w("Stored MQTT settings are invalid; using defaults");
    return factory;
  }
  return s;
}

bool MqttSettingsStore::save(const core::MqttSettings& s) {
  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return false;
  const bool ok =
      putString(prefs, "host", s.host) && prefs.putUShort("port", s.port) > 0 &&
      putString(prefs, "username", s.username) && putString(prefs, "password", s.password) &&
      putString(prefs, "clientId", s.clientId) && putString(prefs, "tCommand", s.topics.command) &&
      putString(prefs, "tPower", s.topics.power) && putString(prefs, "tState", s.topics.state) &&
      putString(prefs, "tSsrTemp", s.topics.ssrTemperature) &&
      putString(prefs, "tTankTemp", s.topics.tankTemperature) &&
      putString(prefs, "tFan", s.topics.fan);
  prefs.end();
  return ok;
}

}  // namespace simpleds::services
