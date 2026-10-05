#include "services/mqtt/mqtt_connection.h"

#include <Arduino.h>

#include <cmath>
#include <string_view>

#include "config/mqtt_defaults.h"
#include "control/power_command.h"

namespace simpleds::services {

namespace {
constexpr const char* kOfflinePayload = "offline";
}  // namespace

bool MqttConnection::begin(const core::MqttSettings& settings, const std::string& lastWillTopic) {
  // The client keeps pointers to the configuration strings: keep our copies.
  settings_ = settings;
  lastWillTopic_ = lastWillTopic;

  esp_mqtt_client_config_t config = {};
  config.host = settings_.host.c_str();
  config.port = settings_.port;
  config.transport = MQTT_TRANSPORT_OVER_TCP;
  config.client_id = settings_.clientId.c_str();
  if (!settings_.username.empty()) {
    config.username = settings_.username.c_str();
    config.password = settings_.password.c_str();
  }
  config.lwt_topic = lastWillTopic_.c_str();
  config.lwt_msg = kOfflinePayload;
  config.lwt_qos = 1;
  config.lwt_retain = 1;
  config.keepalive = config::kMqttKeepAliveS;
  config.reconnect_timeout_ms = config::kMqttReconnectMs;

  client_ = esp_mqtt_client_init(&config);
  if (client_ == nullptr) {
    log_e("Failed to create the MQTT client");
    return false;
  }
  esp_mqtt_client_register_event(client_, MQTT_EVENT_ANY, &MqttConnection::onEvent, this);
  if (esp_mqtt_client_start(client_) != ESP_OK) {
    log_e("Failed to start the MQTT client");
    return false;
  }
  log_i("MQTT connecting to %s:%u", settings_.host.c_str(), settings_.port);
  return true;
}

std::optional<ReceivedCommand> MqttConnection::takeCommand() {
  const int32_t command = pendingCommand_.exchange(kNoCommand);
  if (command == kNoCommand) return std::nullopt;
  if (command == kInvalidCommand) return ReceivedCommand{false, 0.0f};
  return ReceivedCommand{true, command / 10.0f};
}

bool MqttConnection::publish(const std::string& topic, const std::string& payload, bool retain) {
  if (client_ == nullptr || !isConnected()) return false;
  return esp_mqtt_client_enqueue(client_, topic.c_str(), payload.data(), payload.size(), 0,
                                 retain ? 1 : 0, true) >= 0;
}

// Runs in the esp-mqtt task.
void MqttConnection::onEvent(void* handlerArgs, esp_event_base_t, int32_t eventId,
                             void* eventData) {
  auto* self = static_cast<MqttConnection*>(handlerArgs);
  const auto* event = static_cast<esp_mqtt_event_handle_t>(eventData);

  switch (static_cast<esp_mqtt_event_id_t>(eventId)) {
    case MQTT_EVENT_CONNECTED:
      log_i("MQTT connected");
      esp_mqtt_client_subscribe(self->client_, self->settings_.topics.command.c_str(), 1);
      self->connected_.store(true);
      self->connectionCount_.fetch_add(1);
      break;
    case MQTT_EVENT_DISCONNECTED:
      if (self->connected_.exchange(false)) log_w("MQTT disconnected");
      break;
    case MQTT_EVENT_DATA: self->handleData(*event); break;
    default: break;
  }
}

void MqttConnection::handleData(const esp_mqtt_event_t& event) {
  const std::string_view topic(event.topic, event.topic_len);
  if (topic != settings_.topics.command) return;

  // A retained command may be hours old: never act on it.
  if (event.retain) {
    log_w("Ignoring retained command on '%s'", settings_.topics.command.c_str());
    return;
  }

  // Payloads split over several events are far too long to be valid.
  std::optional<float> percent;
  if (event.data_len == event.total_data_len) {
    percent = core::parsePowerPercent(std::string_view(event.data, event.data_len));
  }
  if (!percent) log_w("Invalid power command");
  pendingCommand_.store(percent ? static_cast<int32_t>(std::lround(*percent * 10.0f))
                                : kInvalidCommand);
}

}  // namespace simpleds::services
