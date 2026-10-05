#include "services/provisioning/network_scanner.h"

#include <WiFi.h>

#include <utility>

#include "config/network_defaults.h"

namespace simpleds::services {

namespace {
// With the access point active the radio keeps returning to the AP channel, so
// a scan takes ~12 s. The core aborts async scans after 20x this value.
constexpr uint32_t kAsyncScanMaxMsPerChannel = 1000;
}  // namespace

void NetworkScanner::scanBlocking() {
  const int count = WiFi.scanNetworks();
  finishScan(count);
}

void NetworkScanner::requestScan() {
  if (scanning_) return;
  if (hasResults_ && millis() - lastScanMs_ < config::kNetworkScanMaxAgeMs) return;

  scanning_ = WiFi.scanNetworks(/*async=*/true, /*show_hidden=*/false, /*passive=*/false,
                                kAsyncScanMaxMsPerChannel) == WIFI_SCAN_RUNNING;
  if (!scanning_) log_w("Network scan could not be started");
}

void NetworkScanner::update() {
  if (!scanning_) return;

  const int count = WiFi.scanComplete();
  if (count == WIFI_SCAN_RUNNING) return;

  scanning_ = false;
  finishScan(count);
}

void NetworkScanner::finishScan(int count) {
  lastScanMs_ = millis();
  if (count >= 0) {
    collectResults(count);
  } else {
    log_w("Network scan failed (%d)", count);
  }
  WiFi.scanDelete();
}

void NetworkScanner::collectResults(int count) {
  std::vector<core::ScannedNetwork> found;
  found.reserve(count);
  for (int i = 0; i < count; ++i) {
    found.push_back({WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.encryptionType(i) != WIFI_AUTH_OPEN});
  }
  networks_ = core::prepareNetworkList(std::move(found));
  hasResults_ = true;
  log_i("Network scan found %u networks", static_cast<unsigned>(networks_.size()));
}

}  // namespace simpleds::services
