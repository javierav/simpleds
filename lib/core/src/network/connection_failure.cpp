#include "network/connection_failure.h"

namespace simpleds::core {

namespace {
// Values of wifi_err_reason_t (esp_wifi_types.h), duplicated to keep the core
// independent of ESP-IDF.
constexpr uint8_t kReason4WayHandshakeTimeout = 15;
constexpr uint8_t kReasonNoApFound = 201;
constexpr uint8_t kReasonAuthFail = 202;
constexpr uint8_t kReasonHandshakeTimeout = 204;
}  // namespace

ConnectionFailure classifyDisconnectReason(uint8_t reason) {
  switch (reason) {
    case kReason4WayHandshakeTimeout:
    case kReasonAuthFail:
    case kReasonHandshakeTimeout:
      return ConnectionFailure::kWrongPassword;
    case kReasonNoApFound:
      return ConnectionFailure::kNetworkNotFound;
    default:
      return ConnectionFailure::kOther;
  }
}

const char* failureCode(ConnectionFailure failure) {
  switch (failure) {
    case ConnectionFailure::kNone: return "none";
    case ConnectionFailure::kWrongPassword: return "wrong_password";
    case ConnectionFailure::kNetworkNotFound: return "network_not_found";
    case ConnectionFailure::kTimeout: return "timeout";
    case ConnectionFailure::kOther: return "connection_failed";
  }
  return "connection_failed";
}

}  // namespace simpleds::core
