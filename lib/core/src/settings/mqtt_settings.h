#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include "settings/validation_error.h"

namespace simpleds::core {

inline constexpr std::size_t kMaxHostLength = 64;
inline constexpr std::size_t kMaxMqttUsernameLength = 64;
inline constexpr std::size_t kMaxMqttPasswordLength = 64;
inline constexpr std::size_t kMaxClientIdLength = 64;
inline constexpr std::size_t kMaxTopicLength = 128;

struct MqttTopics {
  std::string command;  // Subscribed: power level in percent.
  std::string power;    // Power level applied, in percent.
  std::string state;    // "active" | "emergency" | "offline" (last will).
  std::string ssrTemperature;
  std::string tankTemperature;
  std::string fan;  // "on" | "off".
};

// Broker connection. MQTT is disabled while the host is empty.
struct MqttSettings {
  std::string host;
  uint16_t port = 0;
  std::string username;  // Empty for anonymous access.
  std::string password;
  std::string clientId;
  MqttTopics topics;
};

inline bool isMqttEnabled(const MqttSettings& settings) { return !settings.host.empty(); }

// A concrete topic name: not empty, no wildcards (+, #) and no NUL.
bool isValidTopic(std::string_view topic);

std::optional<ValidationError> validateMqttSettings(const MqttSettings& settings);

}  // namespace simpleds::core
