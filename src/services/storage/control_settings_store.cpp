#include "services/storage/control_settings_store.h"

#include <Preferences.h>

#include "config/diverter_defaults.h"

namespace simpleds::services {

namespace {
constexpr const char* kNamespace = "control";
constexpr const char* kTripKey = "ssrTripC";
constexpr const char* kResetKey = "ssrResetC";
constexpr const char* kFanOnKey = "fanOnC";
constexpr const char* kFanOffKey = "fanOffC";
constexpr const char* kTelemetryKey = "telemetryS";
constexpr const char* kTimeoutKey = "cmdTimeoutS";

core::ControlSettings defaults() {
  return {config::kDefaultSsrTripC,
          config::kDefaultSsrResetC,
          config::kDefaultFanOnC,
          config::kDefaultFanOffC,
          config::kDefaultTelemetryIntervalS,
          config::kDefaultCommandTimeoutS};
}
}  // namespace

core::ControlSettings ControlSettingsStore::load() {
  const core::ControlSettings factory = defaults();
  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return factory;

  const core::ControlSettings settings{
      prefs.getFloat(kTripKey, factory.ssrTripC),
      prefs.getFloat(kResetKey, factory.ssrResetC),
      prefs.getFloat(kFanOnKey, factory.fanOnC),
      prefs.getFloat(kFanOffKey, factory.fanOffC),
      prefs.getUInt(kTelemetryKey, factory.telemetryIntervalS),
      prefs.getUInt(kTimeoutKey, factory.commandTimeoutS),
  };
  prefs.end();

  if (core::validateControlSettings(settings)) {
    log_w("Stored control settings are invalid; using defaults");
    return factory;
  }
  return settings;
}

bool ControlSettingsStore::save(const core::ControlSettings& settings) {
  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return false;
  const bool ok = prefs.putFloat(kTripKey, settings.ssrTripC) > 0 &&
                  prefs.putFloat(kResetKey, settings.ssrResetC) > 0 &&
                  prefs.putFloat(kFanOnKey, settings.fanOnC) > 0 &&
                  prefs.putFloat(kFanOffKey, settings.fanOffC) > 0 &&
                  prefs.putUInt(kTelemetryKey, settings.telemetryIntervalS) > 0 &&
                  prefs.putUInt(kTimeoutKey, settings.commandTimeoutS) > 0;
  prefs.end();
  return ok;
}

}  // namespace simpleds::services
