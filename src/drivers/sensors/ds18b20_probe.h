#pragma once

#include <DallasTemperature.h>
#include <OneWire.h>

#include <cstdint>
#include <optional>

namespace simpleds::drivers {

// A single DS18B20 on its own 1-Wire bus, sampled without blocking: a
// conversion is started every `intervalMs` and read once it has finished.
class Ds18b20Probe {
 public:
  Ds18b20Probe(uint8_t pin, uint32_t intervalMs);

  void begin();
  void setIntervalMs(uint32_t intervalMs) { intervalMs_ = intervalMs; }

  // Returns true when a new sample (valid or not) has just been taken.
  bool update(uint32_t nowMs);

  // Last sample; nothing if it failed or there is none yet.
  std::optional<float> temperatureC() const { return temperatureC_; }
  uint32_t sampleCount() const { return sampleCount_; }

 private:
  OneWire wire_;
  DallasTemperature sensors_;
  uint32_t intervalMs_;
  uint32_t requestedAtMs_ = 0;
  bool converting_ = false;
  std::optional<float> temperatureC_;
  uint32_t sampleCount_ = 0;
};

}  // namespace simpleds::drivers
