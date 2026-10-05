#include "services/storage/wifi_credentials_store.h"

#include <Preferences.h>

namespace simpleds::services {

namespace {
constexpr const char* kNamespace = "wifi";
constexpr const char* kSsidKey = "ssid";
constexpr const char* kPasswordKey = "password";
}  // namespace

std::optional<core::WifiCredentials> WifiCredentialsStore::load() {
  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return std::nullopt;
  if (!prefs.isKey(kSsidKey)) {
    prefs.end();
    return std::nullopt;
  }

  core::WifiCredentials credentials{prefs.getString(kSsidKey, "").c_str(),
                                    prefs.getString(kPasswordKey, "").c_str()};
  prefs.end();

  if (core::validateCredentials(credentials) != core::CredentialsError::kNone) {
    return std::nullopt;
  }
  return credentials;
}

bool WifiCredentialsStore::save(const core::WifiCredentials& credentials) {
  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return false;

  const bool ok = prefs.putString(kSsidKey, credentials.ssid.c_str()) > 0 &&
                  prefs.putString(kPasswordKey, credentials.password.c_str()) ==
                      credentials.password.size();
  prefs.end();
  return ok;
}

void WifiCredentialsStore::clear() {
  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return;
  prefs.clear();
  prefs.end();
}

}  // namespace simpleds::services
