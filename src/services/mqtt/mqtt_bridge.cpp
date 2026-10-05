#include "services/mqtt/mqtt_bridge.h"

#include "text/number_format.h"

namespace simpleds::services {

namespace {
constexpr const char* kSensorErrorPayload = "error";

std::string temperaturePayload(const std::optional<float>& celsius) {
  return celsius ? core::formatOneDecimal(*celsius) : kSensorErrorPayload;
}
}  // namespace

MqttBridge::MqttBridge(MqttConnection& connection, Diverter& diverter)
    : connection_(connection), diverter_(diverter) {}

void MqttBridge::begin(const core::MqttSettings& settings) {
  settings_ = settings;
  enabled_ = core::isMqttEnabled(settings);
  if (!enabled_) log_i("MQTT disabled: no broker configured");
}

void MqttBridge::update(bool networkUp) {
  if (!enabled_) return;
  if (!started_) {
    if (!networkUp) return;
    started_ = connection_.begin(settings_, settings_.topics.state);
    if (!started_) enabled_ = false;
    return;
  }

  const bool connected = connection_.isConnected();
  if (wasConnected_ && !connected) {
    // Without the controller there is no one to stop the load.
    diverter_.dropCommand();
  }
  wasConnected_ = connected;

  forwardCommands();
  if (!connected) return;

  // Republish everything after each (re)connection.
  const uint32_t connection = connection_.connectionCount();
  const bool all = connection != publishedConnection_;
  publishedConnection_ = connection;
  publish(diverter_.status(), all);
}

void MqttBridge::forwardCommands() {
  const std::optional<ReceivedCommand> command = connection_.takeCommand();
  if (!command) return;
  if (command->valid) {
    diverter_.handleCommand(command->percent);
  } else {
    diverter_.dropCommand();
  }
}

void MqttBridge::publish(const DiverterStatus& status, bool all) {
  const core::MqttTopics& topics = settings_.topics;
  publishIfChanged(topics.state, lastState_, core::stateCode(status.state), all);
  publishIfChanged(topics.power, lastPower_, core::formatOneDecimal(status.outputPercent), all);
  publishIfChanged(topics.fan, lastFan_, status.fanOn ? "on" : "off", all);

  // Temperatures go out once per sample (i.e. every telemetry interval for
  // the tank, together with the latest SSR reading).
  if (status.tankSamples > 0 && (all || status.tankSamples != publishedTankSamples_)) {
    publishedTankSamples_ = status.tankSamples;
    connection_.publish(topics.tankTemperature, temperaturePayload(status.tankTemperatureC), true);
    if (status.ssrSamples > 0) {
      connection_.publish(topics.ssrTemperature, temperaturePayload(status.ssrTemperatureC), true);
    }
  }
}

void MqttBridge::publishIfChanged(const std::string& topic, std::string& last,
                                  const std::string& value, bool force) {
  if (!force && value == last) return;
  if (connection_.publish(topic, value, true)) last = value;
}

MqttBridge::State MqttBridge::state() const {
  if (!enabled_) return State::kDisabled;
  return connection_.isConnected() ? State::kConnected : State::kConnecting;
}

const char* bridgeStateCode(MqttBridge::State state) {
  switch (state) {
    case MqttBridge::State::kDisabled: return "disabled";
    case MqttBridge::State::kConnecting: return "connecting";
    case MqttBridge::State::kConnected: return "connected";
  }
  return "unknown";
}

}  // namespace simpleds::services
