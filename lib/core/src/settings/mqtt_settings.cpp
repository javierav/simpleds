#include "settings/mqtt_settings.h"

#include <cctype>
#include <iterator>

namespace simpleds::core {

namespace {
bool isValidHost(std::string_view host) {
  if (host.size() > kMaxHostLength) return false;
  for (const char c : host) {
    const auto u = static_cast<unsigned char>(c);
    if (!std::isalnum(u) && c != '.' && c != '-' && c != ':') return false;
  }
  return true;
}

bool isValidClientId(std::string_view clientId) {
  if (clientId.empty() || clientId.size() > kMaxClientIdLength) return false;
  for (const char c : clientId) {
    if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-' && c != '_') return false;
  }
  return true;
}
}  // namespace

bool isValidTopic(std::string_view topic) {
  if (topic.empty() || topic.size() > kMaxTopicLength) return false;
  for (const char c : topic) {
    if (c == '+' || c == '#' || c == '\0') return false;
  }
  return true;
}

std::optional<ValidationError> validateMqttSettings(const MqttSettings& settings) {
  if (!isValidHost(settings.host)) return ValidationError{"host", "invalid"};
  if (settings.port == 0) return ValidationError{"port", "out_of_range"};
  if (settings.username.size() > kMaxMqttUsernameLength) {
    return ValidationError{"username", "too_long"};
  }
  if (settings.password.size() > kMaxMqttPasswordLength) {
    return ValidationError{"password", "too_long"};
  }
  if (!isValidClientId(settings.clientId)) return ValidationError{"clientId", "invalid"};

  const MqttTopics& t = settings.topics;
  const struct {
    const char* field;
    const std::string& topic;
  } published[] = {{"powerTopic", t.power},
                   {"stateTopic", t.state},
                   {"ssrTemperatureTopic", t.ssrTemperature},
                   {"tankTemperatureTopic", t.tankTemperature},
                   {"fanTopic", t.fan}};

  if (!isValidTopic(t.command)) return ValidationError{"commandTopic", "invalid"};
  for (const auto& p : published) {
    if (!isValidTopic(p.topic)) return ValidationError{p.field, "invalid"};
    // The device would otherwise receive its own messages as commands.
    if (p.topic == t.command) return ValidationError{p.field, "same_as_command"};
  }
  for (std::size_t i = 0; i < std::size(published); ++i) {
    for (std::size_t j = i + 1; j < std::size(published); ++j) {
      if (published[i].topic == published[j].topic) {
        return ValidationError{published[j].field, "duplicated"};
      }
    }
  }
  return std::nullopt;
}

}  // namespace simpleds::core
