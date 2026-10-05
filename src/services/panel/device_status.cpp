#include "services/panel/device_status.h"

#include <WiFi.h>

#include "config/build_info.h"
#include "config/network_defaults.h"
#include "network/scanned_network.h"
#include "text/json_escape.h"

namespace simpleds::services {

DeviceStatus readDeviceStatus() {
  return {
      config::kFirmwareVersion,
      std::string(config::kHostname) + ".local",
      WiFi.SSID().c_str(),
      WiFi.localIP().toString().c_str(),
      WiFi.RSSI(),
      static_cast<uint32_t>(millis() / 1000),
      ESP.getFreeHeap(),
  };
}

std::string deviceStatusToJson(const DeviceStatus& status) {
  return "{\"firmware\":\"" + core::escapeJson(status.firmwareVersion) + "\",\"hostname\":\"" +
         core::escapeJson(status.hostname) + "\",\"ssid\":\"" + core::escapeJson(status.ssid) +
         "\",\"ip\":\"" + core::escapeJson(status.ip) + "\",\"rssi\":" +
         std::to_string(status.rssi) + ",\"bars\":" +
         std::to_string(core::signalBars(status.rssi)) + ",\"uptime\":" +
         std::to_string(status.uptimeS) + ",\"freeHeap\":" + std::to_string(status.freeHeapBytes) +
         "}";
}

}  // namespace simpleds::services
