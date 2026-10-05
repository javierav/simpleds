#pragma once

#include "settings/control_settings.h"

namespace simpleds::services {

// Persists the diverter settings in NVS. Missing or invalid values fall back
// to the factory defaults in config/diverter_defaults.h.
class ControlSettingsStore {
 public:
  core::ControlSettings load();
  bool save(const core::ControlSettings& settings);
};

}  // namespace simpleds::services
