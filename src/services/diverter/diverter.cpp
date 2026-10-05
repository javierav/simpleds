#include "services/diverter/diverter.h"

#include <Arduino.h>

#include "config/diverter_defaults.h"

namespace simpleds::services {

namespace {
uint32_t secondsToMs(uint32_t seconds) { return seconds * 1000; }
}  // namespace

Diverter::Diverter(core::ILoadOutput& load, drivers::FanOutput& fan,
                   drivers::Ds18b20Probe& ssrProbe, drivers::Ds18b20Probe& tankProbe)
    : load_(load),
      fan_(fan),
      ssrProbe_(ssrProbe),
      tankProbe_(tankProbe),
      guard_(config::kDefaultSsrTripC, config::kDefaultSsrResetC, config::kSsrProbeMaxErrors),
      fanSwitch_(config::kDefaultFanOnC, config::kDefaultFanOffC),
      commands_(secondsToMs(config::kDefaultCommandTimeoutS)) {}

void Diverter::begin(const core::ControlSettings& settings) {
  applySettings(settings);
  load_.setLevel(0.0f);
}

void Diverter::applySettings(const core::ControlSettings& settings) {
  guard_.setThresholds(settings.ssrTripC, settings.ssrResetC);
  fanSwitch_.setThresholds(settings.fanOnC, settings.fanOffC);
  commands_.setTimeoutMs(secondsToMs(settings.commandTimeoutS));
  tankProbe_.setIntervalMs(secondsToMs(settings.telemetryIntervalS));
}

void Diverter::update() {
  const uint32_t now = millis();
  if (ssrProbe_.update(now)) updateSsrTemperature();
  tankProbe_.update(now);

  const std::optional<float> command = commands_.activePercent(now);
  outputPercent_ = guard_.allowsOutput() && command ? *command : 0.0f;
  // Refreshed on every pass: SsrOutput turns itself off if this stops.
  load_.setLevel(outputPercent_ / 100.0f);

  fan_.set(guard_.state() == core::ThermalState::kEmergency || fanSwitch_.isOn());
  logChanges(command.has_value());
}

void Diverter::updateSsrTemperature() {
  const std::optional<float> temperature = ssrProbe_.temperatureC();
  guard_.update(temperature);
  if (temperature) fanSwitch_.update(*temperature);
}

void Diverter::handleCommand(float percent) { commands_.accept(percent, millis()); }

void Diverter::dropCommand() { commands_.clear(); }

void Diverter::logChanges(bool hasCommand) {
  if (guard_.state() != loggedState_) {
    loggedState_ = guard_.state();
    if (loggedState_ == core::ThermalState::kEmergency) {
      log_w("Emergency (%s): load off", core::reasonCode(guard_.reason()));
    } else {
      log_i("Diverter %s", core::stateCode(loggedState_));
    }
  }
  if (hadCommand_ && !hasCommand) log_w("No valid power command: load off");
  hadCommand_ = hasCommand;
}

DiverterStatus Diverter::status() const {
  return {guard_.state(),
          guard_.reason(),
          outputPercent_,
          commands_.activePercent(millis()),
          ssrProbe_.temperatureC(),
          tankProbe_.temperatureC(),
          ssrProbe_.sampleCount(),
          tankProbe_.sampleCount(),
          fan_.isOn()};
}

}  // namespace simpleds::services
