#include "services/web/web_assets.h"

#include <cstring>

namespace simpleds::services {

const WebAsset* findWebAsset(const char* path) {
  for (size_t i = 0; i < kWebAssetCount; ++i) {
    if (std::strcmp(kWebAssets[i].path, path) == 0) return &kWebAssets[i];
  }
  return nullptr;
}

}  // namespace simpleds::services
