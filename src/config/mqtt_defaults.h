#pragma once

#include <cstdint>

namespace simpleds::config {

// Factory values; the broker host is empty, so MQTT stays off until set.
inline constexpr uint16_t kDefaultMqttPort = 1883;
inline constexpr const char* kDefaultCommandTopic = "simpleds/in/power";
inline constexpr const char* kDefaultPowerTopic = "simpleds/out/power";
inline constexpr const char* kDefaultStateTopic = "simpleds/out/state";
inline constexpr const char* kDefaultSsrTemperatureTopic = "simpleds/out/ssr/temperature";
inline constexpr const char* kDefaultTankTemperatureTopic = "simpleds/out/tank/temperature";
inline constexpr const char* kDefaultFanTopic = "simpleds/out/fan";

inline constexpr uint16_t kMqttKeepAliveS = 30;
inline constexpr uint32_t kMqttReconnectMs = 10000;

}  // namespace simpleds::config
