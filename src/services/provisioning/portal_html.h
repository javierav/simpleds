#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "network/scanned_network.h"
#include "network/wifi_credentials.h"

namespace simpleds::services {

// Form field names. An empty kSsidField means "other network", typed in
// kOtherSsidField.
inline constexpr const char* kSsidField = "ssid";
inline constexpr const char* kOtherSsidField = "other";
inline constexpr const char* kPasswordField = "password";

std::string renderConfigPage(const std::vector<core::ScannedNetwork>& networks,
                             std::string_view selectedSsid, std::string_view errorMessage);

std::string renderSavedPage(std::string_view ssid);

std::string renderErrorPage(std::string_view message);

// User-facing description of a credentials validation error.
const char* describeError(core::CredentialsError error);

}  // namespace simpleds::services
