#include "network/scanned_network.h"

#include <algorithm>

namespace simpleds::core {

std::vector<ScannedNetwork> prepareNetworkList(std::vector<ScannedNetwork> networks) {
  std::stable_sort(networks.begin(), networks.end(),
                   [](const auto& a, const auto& b) { return a.rssi > b.rssi; });

  std::vector<ScannedNetwork> result;
  for (auto& network : networks) {
    if (network.ssid.empty()) continue;
    const bool seen = std::any_of(result.begin(), result.end(),
                                  [&](const auto& r) { return r.ssid == network.ssid; });
    if (!seen) result.push_back(std::move(network));
  }
  return result;
}

int signalBars(int32_t rssi) {
  if (rssi >= -55) return 4;
  if (rssi >= -65) return 3;
  if (rssi >= -75) return 2;
  return 1;
}

}  // namespace simpleds::core
