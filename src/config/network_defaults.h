#pragma once

#include <cstdint>

namespace simpleds::config {

inline constexpr const char* kHostname = "simpleds";

// The access point name is this prefix plus the last 2 bytes of the MAC.
inline constexpr const char* kAccessPointSsidPrefix = "SimpleDS-";

inline constexpr uint32_t kWifiReconnectIntervalMs = 15000;
inline constexpr uint32_t kNetworkScanMaxAgeMs = 10000;
inline constexpr uint32_t kRestartDelayMs = 1500;

// Testing new credentials from the portal before storing them.
inline constexpr uint32_t kConnectionTestTimeoutMs = 30000;
inline constexpr uint8_t kConnectionTestMaxAttempts = 2;
// Delay before restarting after a successful test, so the phone (which may
// have to rejoin the AP after a channel change) can still read the result.
inline constexpr uint32_t kRestartAfterProvisioningMs = 10000;

// Holding the reset button this long forgets the configured WiFi network.
inline constexpr uint32_t kWifiResetHoldMs = 5000;

}  // namespace simpleds::config
