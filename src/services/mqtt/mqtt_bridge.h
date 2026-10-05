#pragma once

#include <cstdint>
#include <string>

#include "services/diverter/diverter.h"
#include "services/mqtt/mqtt_connection.h"
#include "settings/mqtt_settings.h"

namespace simpleds::services {

// Connects the diverter with the external controller over MQTT: forwards the
// power commands to the diverter and publishes its state, applied power, fan
// and temperatures (retained, so new subscribers get the latest values).
class MqttBridge {
 public:
  enum class State { kDisabled, kConnecting, kConnected };

  MqttBridge(MqttConnection& connection, Diverter& diverter);

  void begin(const core::MqttSettings& settings);
  // networkUp: the device has an IP address; MQTT starts on the first one.
  void update(bool networkUp);

  State state() const;

 private:
  void forwardCommands();
  void publish(const DiverterStatus& status, bool all);
  void publishIfChanged(const std::string& topic, std::string& last, const std::string& value,
                        bool force);

  MqttConnection& connection_;
  Diverter& diverter_;
  core::MqttSettings settings_;
  bool enabled_ = false;
  bool started_ = false;
  bool wasConnected_ = false;
  uint32_t publishedConnection_ = 0;
  uint32_t publishedTankSamples_ = 0;
  std::string lastState_;
  std::string lastPower_;
  std::string lastFan_;
};

const char* bridgeStateCode(MqttBridge::State state);

}  // namespace simpleds::services
