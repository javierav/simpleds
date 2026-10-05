#include "services/system/restart_scheduler.h"

#include <Arduino.h>

namespace simpleds::services {

void RestartScheduler::schedule(uint32_t delayMs) {
  scheduled_ = true;
  scheduledAtMs_ = millis();
  delayMs_ = delayMs;
  log_i("Restart scheduled in %u ms", delayMs);
}

void RestartScheduler::update() {
  if (scheduled_ && millis() - scheduledAtMs_ >= delayMs_) {
    log_i("Restarting");
    ESP.restart();
  }
}

}  // namespace simpleds::services
