#pragma once

#include <cstdint>
#include <vector>

#include "network/scanned_network.h"

namespace simpleds::services {

// Scans nearby WiFi networks and caches them, strongest signal first.
class NetworkScanner {
 public:
  // Scans synchronously (~7 s). Only for setup(), before the AP starts.
  void scanBlocking();

  // Starts an async scan unless one is running or the results are recent.
  void requestScan();
  void update();
  const std::vector<core::ScannedNetwork>& networks() const { return networks_; }
  bool isScanning() const { return scanning_; }

 private:
  void finishScan(int count);
  void collectResults(int count);

  std::vector<core::ScannedNetwork> networks_;
  uint32_t lastScanMs_ = 0;
  bool scanning_ = false;
  bool hasResults_ = false;
};

}  // namespace simpleds::services
