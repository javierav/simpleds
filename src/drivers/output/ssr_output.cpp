#include "drivers/output/ssr_output.h"

#include <Arduino.h>

#include <algorithm>
#include <cmath>

namespace simpleds::drivers {

SsrOutput::SsrOutput(uint8_t pin, uint32_t mainsFrequencyHz, uint32_t leaseMs)
    : pin_(pin), cyclePeriodUs_(1000000 / mainsFrequencyHz), leaseMs_(leaseMs) {}

bool SsrOutput::begin() {
  digitalWrite(pin_, LOW);
  pinMode(pin_, OUTPUT);

  // ESP_TIMER_TASK runs the callback in the high-priority esp_timer task, so
  // it is not delayed by the main loop and has no ISR restrictions.
  const esp_timer_create_args_t args = {
      .callback = &SsrOutput::onCycle,
      .arg = this,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "ssr",
      .skip_unhandled_events = true,
  };
  if (esp_timer_create(&args, &timer_) != ESP_OK ||
      esp_timer_start_periodic(timer_, cyclePeriodUs_) != ESP_OK) {
    log_e("Failed to start the SSR timer; the load stays off");
    return false;
  }
  return true;
}

void SsrOutput::setLevel(float level) {
  const float clamped = std::clamp(std::isfinite(level) ? level : 0.0f, 0.0f, 1.0f);
  levelPermille_.store(static_cast<uint16_t>(std::lround(clamped * core::kFullLevelPermille)));
  refreshedAtMs_.store(millis());
}

void SsrOutput::onCycle(void* arg) {
  auto* self = static_cast<SsrOutput*>(arg);
  const bool leaseExpired = millis() - self->refreshedAtMs_.load() >= self->leaseMs_;
  const uint16_t level = leaseExpired ? 0 : self->levelPermille_.load();
  digitalWrite(self->pin_, self->modulator_.nextCycle(level) ? HIGH : LOW);
}

}  // namespace simpleds::drivers
