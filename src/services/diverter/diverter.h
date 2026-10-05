#pragma once

#include "control/command_tracker.h"
#include "drivers/output/fan_output.h"
#include "drivers/sensors/ds18b20_probe.h"
#include "output/load_output.h"
#include "safety/hysteresis_switch.h"
#include "safety/thermal_guard.h"
#include "services/diverter/diverter_status.h"
#include "settings/control_settings.h"

namespace simpleds::services {

// Control loop of the diverter: applies the power commands received from the
// external controller to the load, as long as the SSR thermal protection
// allows it, and drives the cooling fan. Any doubt switches the load off.
class Diverter {
 public:
  Diverter(core::ILoadOutput& load, drivers::FanOutput& fan, drivers::Ds18b20Probe& ssrProbe,
           drivers::Ds18b20Probe& tankProbe);

  void begin(const core::ControlSettings& settings);
  void applySettings(const core::ControlSettings& settings);
  void update();

  void handleCommand(float percent);
  // Switches the load off until the next valid command.
  void dropCommand();

  DiverterStatus status() const;

 private:
  void updateSsrTemperature();
  void logChanges(bool hasCommand);

  core::ILoadOutput& load_;
  drivers::FanOutput& fan_;
  drivers::Ds18b20Probe& ssrProbe_;
  drivers::Ds18b20Probe& tankProbe_;
  core::ThermalGuard guard_;
  core::HysteresisSwitch fanSwitch_;
  core::CommandTracker commands_;
  float outputPercent_ = 0.0f;
  core::ThermalState loggedState_ = core::ThermalState::kStarting;
  bool hadCommand_ = false;
};

}  // namespace simpleds::services
