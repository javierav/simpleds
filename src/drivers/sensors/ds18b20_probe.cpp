#include "drivers/sensors/ds18b20_probe.h"

#include "sensors/ds18b20_reading.h"

namespace simpleds::drivers {

namespace {
// Conversion time at the default 12-bit resolution.
constexpr uint32_t kConversionMs = 750;
}  // namespace

Ds18b20Probe::Ds18b20Probe(uint8_t pin, uint32_t intervalMs)
    : wire_(pin), sensors_(&wire_), intervalMs_(intervalMs) {}

void Ds18b20Probe::begin() {
  sensors_.begin();
  sensors_.setWaitForConversion(false);
}

bool Ds18b20Probe::update(uint32_t nowMs) {
  if (converting_) {
    if (nowMs - requestedAtMs_ < kConversionMs) return false;
    converting_ = false;
    temperatureC_ = core::validateDs18b20Reading(sensors_.getTempCByIndex(0));
    ++sampleCount_;
    return true;
  }

  if (sampleCount_ > 0 && nowMs - requestedAtMs_ < intervalMs_) return false;
  // Detects a probe connected after boot (or replaced).
  if (sensors_.getDeviceCount() == 0) sensors_.begin();
  sensors_.requestTemperatures();
  requestedAtMs_ = nowMs;
  converting_ = true;
  return false;
}

}  // namespace simpleds::drivers
