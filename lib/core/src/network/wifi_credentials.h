#pragma once

#include <cstddef>
#include <string>

namespace simpleds::core {

// Limits defined by IEEE 802.11 (SSID) and WPA2-PSK (passphrase).
inline constexpr std::size_t kMaxSsidLength = 32;
inline constexpr std::size_t kMinPasswordLength = 8;
inline constexpr std::size_t kMaxPasswordLength = 63;

struct WifiCredentials {
  std::string ssid;
  std::string password;  // Empty for open networks.
};

enum class CredentialsError {
  kNone,
  kSsidEmpty,
  kSsidTooLong,
  kPasswordTooShort,
  kPasswordTooLong,
};

CredentialsError validateCredentials(const WifiCredentials& credentials);

// Stable machine-readable code for the error, used by the web API.
const char* errorCode(CredentialsError error);

}  // namespace simpleds::core
