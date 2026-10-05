#include "services/web/web_responses.h"

#include "services/web/web_assets.h"

namespace simpleds::services {

bool sendWebAsset(WebServer& server, const char* path) {
  const WebAsset* asset = findWebAsset(path);
  if (asset == nullptr) return false;

  server.sendHeader("Content-Encoding", "gzip");
  server.sendHeader("Cache-Control", "no-cache");
  server.send_P(200, asset->contentType, reinterpret_cast<const char*>(asset->data),
                asset->size);
  return true;
}

void sendJson(WebServer& server, int status, const std::string& json) {
  server.sendHeader("Cache-Control", "no-store");
  server.send(status, "application/json", json.c_str());
}

}  // namespace simpleds::services
