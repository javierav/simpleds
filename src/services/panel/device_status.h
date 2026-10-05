#pragma once

#include <cstdint>
#include <string>

namespace simpleds::services {

struct DeviceStatus {
  std::string firmwareVersion;
  std::string hostname;
  std::string ssid;
  std::string ip;
  int32_t rssi;  // dBm
  uint32_t uptimeS;
  uint32_t freeHeapBytes;
};

// Reads the current status of the device.
DeviceStatus readDeviceStatus();

// {"firmware","hostname","ssid","ip","rssi","bars","uptime","freeHeap"}
std::string deviceStatusToJson(const DeviceStatus& status);

}  // namespace simpleds::services
