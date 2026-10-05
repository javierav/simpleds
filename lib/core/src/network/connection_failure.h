#pragma once

#include <cstdint>

namespace simpleds::core {

enum class ConnectionFailure {
  kNone,
  kWrongPassword,
  kNetworkNotFound,
  kTimeout,
  kOther,
};

// Maps an ESP-IDF station disconnect reason (wifi_err_reason_t) to a failure
// the user can act on.
ConnectionFailure classifyDisconnectReason(uint8_t reason);

// Stable machine-readable code for the failure, used by the web API.
const char* failureCode(ConnectionFailure failure);

}  // namespace simpleds::core
