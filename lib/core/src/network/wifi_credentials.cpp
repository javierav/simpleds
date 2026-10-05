#include "network/wifi_credentials.h"

namespace simpleds::core {

CredentialsError validateCredentials(const WifiCredentials& credentials) {
  if (credentials.ssid.empty()) return CredentialsError::kSsidEmpty;
  if (credentials.ssid.size() > kMaxSsidLength) return CredentialsError::kSsidTooLong;

  const std::size_t passwordLength = credentials.password.size();
  if (passwordLength == 0) return CredentialsError::kNone;
  if (passwordLength < kMinPasswordLength) return CredentialsError::kPasswordTooShort;
  if (passwordLength > kMaxPasswordLength) return CredentialsError::kPasswordTooLong;
  return CredentialsError::kNone;
}

const char* errorCode(CredentialsError error) {
  switch (error) {
    case CredentialsError::kNone: return "none";
    case CredentialsError::kSsidEmpty: return "ssid_empty";
    case CredentialsError::kSsidTooLong: return "ssid_too_long";
    case CredentialsError::kPasswordTooShort: return "password_too_short";
    case CredentialsError::kPasswordTooLong: return "password_too_long";
  }
  return "invalid";
}

}  // namespace simpleds::core
