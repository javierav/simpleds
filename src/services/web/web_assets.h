#pragma once

#include <cstddef>
#include <cstdint>

namespace simpleds::services {

// A file from web/, gzip-compressed and embedded in flash at build time.
struct WebAsset {
  const char* path;  // URL path, e.g. "/portal/index.html".
  const char* contentType;
  const uint8_t* data;
  size_t size;
};

// Defined in src/generated/web_assets_data.cpp (scripts/embed_web_assets.py).
extern const WebAsset kWebAssets[];
extern const size_t kWebAssetCount;

const WebAsset* findWebAsset(const char* path);

}  // namespace simpleds::services
