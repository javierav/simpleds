#pragma once

#include <string>
#include <vector>

#include "network/scanned_network.h"

namespace simpleds::services {

// {"scanning":bool,"networks":[{"ssid":string,"bars":1-4,"secured":bool}]}
std::string networksToJson(const std::vector<core::ScannedNetwork>& networks, bool scanning);

// {"error":string}
std::string errorToJson(const char* code);

// {"status":"idle"|"testing"|"failed","error":string?}
std::string testStatusToJson(const char* status, const char* errorCode);

// {"status":"connected","ip":string,"hostname":string}
std::string testSuccessToJson(const std::string& ip, const std::string& hostname);

}  // namespace simpleds::services
