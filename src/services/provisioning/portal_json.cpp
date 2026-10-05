#include "services/provisioning/portal_json.h"

#include "text/json_escape.h"

namespace simpleds::services {

std::string networksToJson(const std::vector<core::ScannedNetwork>& networks, bool scanning) {
  std::string json =
      std::string("{\"scanning\":") + (scanning ? "true" : "false") + ",\"networks\":[";
  for (size_t i = 0; i < networks.size(); ++i) {
    const auto& network = networks[i];
    if (i > 0) json += ',';
    json += "{\"ssid\":\"" + core::escapeJson(network.ssid) +
            "\",\"bars\":" + std::to_string(core::signalBars(network.rssi)) +
            ",\"secured\":" + (network.secured ? "true" : "false") + "}";
  }
  json += "]}";
  return json;
}

std::string errorToJson(const char* code) {
  return std::string("{\"error\":\"") + core::escapeJson(code) + "\"}";
}

std::string testStatusToJson(const char* status, const char* errorCode) {
  std::string json = std::string("{\"status\":\"") + core::escapeJson(status) + "\"";
  if (errorCode != nullptr) json += ",\"error\":\"" + core::escapeJson(errorCode) + "\"";
  return json + "}";
}

std::string testSuccessToJson(const std::string& ip, const std::string& hostname) {
  return "{\"status\":\"connected\",\"ip\":\"" + core::escapeJson(ip) + "\",\"hostname\":\"" +
         core::escapeJson(hostname) + "\"}";
}

}  // namespace simpleds::services
