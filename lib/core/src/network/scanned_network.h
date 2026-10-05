#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace simpleds::core {

struct ScannedNetwork {
  std::string ssid;
  int32_t rssi;  // dBm
  bool secured;
};

// Drops hidden networks and duplicate SSIDs (keeping the strongest access
// point) and sorts by signal strength, strongest first.
std::vector<ScannedNetwork> prepareNetworkList(std::vector<ScannedNetwork> networks);

// Signal strength as 1–4 bars.
int signalBars(int32_t rssi);

}  // namespace simpleds::core
