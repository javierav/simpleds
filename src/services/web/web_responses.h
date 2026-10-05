#pragma once

#include <WebServer.h>

#include <string>

namespace simpleds::services {

// Sends the embedded asset at `path`. Returns false if there is none.
bool sendWebAsset(WebServer& server, const char* path);

void sendJson(WebServer& server, int status, const std::string& json);

}  // namespace simpleds::services
