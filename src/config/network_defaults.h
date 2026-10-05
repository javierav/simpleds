#pragma once

#include <cstdint>

namespace simpleds::config {

inline constexpr const char* kHostname = "simpleds";

// The access point name is this prefix plus the last 2 bytes of the MAC.
inline constexpr const char* kAccessPointSsidPrefix = "SimpleDS-";

inline constexpr uint32_t kWifiReconnectIntervalMs = 15000;
inline constexpr uint32_t kNetworkScanMaxAgeMs = 10000;
inline constexpr uint32_t kRestartDelayMs = 1500;

// Holding the reset button this long forgets the configured WiFi network.
inline constexpr uint32_t kWifiResetHoldMs = 5000;

}  // namespace simpleds::config
