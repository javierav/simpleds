#include "services/panel/settings_json.h"

#include <optional>

#include "text/json_escape.h"
#include "text/number_format.h"

namespace simpleds::services {

namespace {
std::string quoted(const std::string& text) { return "\"" + core::escapeJson(text) + "\""; }

std::string number(float value) { return core::formatOneDecimal(value); }

std::string optionalNumber(const std::optional<float>& value) {
  return value ? number(*value) : "null";
}

std::string boolean(bool value) { return value ? "true" : "false"; }
}  // namespace

std::string controlSettingsToJson(const core::ControlSettings& s) {
  return "{\"ssrTripC\":" + number(s.ssrTripC) + ",\"ssrResetC\":" + number(s.ssrResetC) +
         ",\"fanOnC\":" + number(s.fanOnC) + ",\"fanOffC\":" + number(s.fanOffC) +
         ",\"telemetryIntervalS\":" + std::to_string(s.telemetryIntervalS) +
         ",\"commandTimeoutS\":" + std::to_string(s.commandTimeoutS) + "}";
}

std::string mqttSettingsToJson(const core::MqttSettings& s) {
  return "{\"host\":" + quoted(s.host) + ",\"port\":" + std::to_string(s.port) +
         ",\"username\":" + quoted(s.username) +
         ",\"hasPassword\":" + boolean(!s.password.empty()) +
         ",\"clientId\":" + quoted(s.clientId) + ",\"commandTopic\":" + quoted(s.topics.command) +
         ",\"powerTopic\":" + quoted(s.topics.power) + ",\"stateTopic\":" + quoted(s.topics.state) +
         ",\"ssrTemperatureTopic\":" + quoted(s.topics.ssrTemperature) +
         ",\"tankTemperatureTopic\":" + quoted(s.topics.tankTemperature) +
         ",\"fanTopic\":" + quoted(s.topics.fan) + "}";
}

std::string diverterStatusToJson(const DiverterStatus& s) {
  return std::string("{\"state\":\"") + core::stateCode(s.state) + "\",\"reason\":\"" +
         core::reasonCode(s.reason) + "\",\"output\":" + number(s.outputPercent) +
         ",\"command\":" + optionalNumber(s.commandPercent) +
         ",\"ssrTemperature\":" + optionalNumber(s.ssrTemperatureC) +
         ",\"ssrSampled\":" + boolean(s.ssrSamples > 0) +
         ",\"tankTemperature\":" + optionalNumber(s.tankTemperatureC) +
         ",\"tankSampled\":" + boolean(s.tankSamples > 0) + ",\"fan\":" + boolean(s.fanOn) + "}";
}

std::string validationErrorToJson(const core::ValidationError& error) {
  return std::string("{\"error\":\"invalid\",\"field\":\"") + error.field + "\",\"code\":\"" +
         error.code + "\"}";
}

}  // namespace simpleds::services
