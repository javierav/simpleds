#include "services/panel/settings_form.h"

#include <cstdlib>
#include <string>

namespace simpleds::services {

namespace {
std::string text(WebServer& server, const char* name) {
  String value = server.arg(name);
  value.trim();
  return value.c_str();
}

float decimal(WebServer& server, const char* name) {
  const std::string value = text(server, name);
  char* end = nullptr;
  const float number = std::strtof(value.c_str(), &end);
  return value.empty() || *end != '\0' ? -1.0f : number;
}

uint32_t integer(WebServer& server, const char* name) {
  const std::string value = text(server, name);
  char* end = nullptr;
  const unsigned long number = std::strtoul(value.c_str(), &end, 10);
  return value.empty() || *end != '\0' || number > UINT32_MAX ? 0 : number;
}
}  // namespace

core::ControlSettings controlSettingsFromForm(WebServer& server) {
  return {decimal(server, "ssrTripC"),
          decimal(server, "ssrResetC"),
          decimal(server, "fanOnC"),
          decimal(server, "fanOffC"),
          integer(server, "telemetryIntervalS"),
          integer(server, "commandTimeoutS")};
}

core::MqttSettings mqttSettingsFromForm(WebServer& server, const core::MqttSettings& current) {
  core::MqttSettings s;
  s.host = text(server, "host");
  const uint32_t port = integer(server, "port");
  s.port = port <= UINT16_MAX ? port : 0;
  s.username = text(server, "username");
  // Passwords are taken verbatim: spaces may be part of them.
  s.password = server.arg("password").c_str();
  if (s.password.empty() && !s.username.empty()) s.password = current.password;
  s.clientId = text(server, "clientId");
  s.topics.command = text(server, "commandTopic");
  s.topics.power = text(server, "powerTopic");
  s.topics.state = text(server, "stateTopic");
  s.topics.ssrTemperature = text(server, "ssrTemperatureTopic");
  s.topics.tankTemperature = text(server, "tankTemperatureTopic");
  s.topics.fan = text(server, "fanTopic");
  return s;
}

}  // namespace simpleds::services
