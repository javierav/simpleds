#pragma once

#include <mqtt_client.h>

#include <atomic>
#include <cstdint>
#include <optional>
#include <string>

#include "settings/mqtt_settings.h"

namespace simpleds::services {

// A power command as received: `valid` is false for unparseable payloads.
struct ReceivedCommand {
  bool valid;
  float percent;
};

// Connection to the MQTT broker on top of ESP-IDF's esp-mqtt client, which
// runs in its own task and reconnects by itself. Events from that task reach
// the main loop only through atomics.
class MqttConnection {
 public:
  // Starts connecting. `lastWillTopic` receives "offline" (retained) when the
  // broker loses the device.
  bool begin(const core::MqttSettings& settings, const std::string& lastWillTopic);

  bool isConnected() const { return connected_.load(); }
  // Increases on every successful (re)connection.
  uint32_t connectionCount() const { return connectionCount_.load(); }

  // Latest command received on the command topic since the previous call.
  std::optional<ReceivedCommand> takeCommand();

  // Queues a message; sent by the client task without blocking the caller.
  bool publish(const std::string& topic, const std::string& payload, bool retain);

 private:
  static void onEvent(void* handlerArgs, esp_event_base_t base, int32_t eventId, void* eventData);
  void handleData(const esp_mqtt_event_t& event);

  // Pending command in permille, or one of these markers.
  static constexpr int32_t kNoCommand = -1;
  static constexpr int32_t kInvalidCommand = -2;

  core::MqttSettings settings_;
  std::string lastWillTopic_;
  esp_mqtt_client_handle_t client_ = nullptr;
  std::atomic<bool> connected_{false};
  std::atomic<uint32_t> connectionCount_{0};
  std::atomic<int32_t> pendingCommand_{kNoCommand};
};

}  // namespace simpleds::services
