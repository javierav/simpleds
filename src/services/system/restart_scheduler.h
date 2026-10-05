#pragma once

#include <cstdint>

namespace simpleds::services {

// Restarts the device after a delay, so pending work (e.g. an HTTP response)
// can complete first.
class RestartScheduler {
 public:
  void schedule(uint32_t delayMs);
  void update();

 private:
  bool scheduled_ = false;
  uint32_t scheduledAtMs_ = 0;
  uint32_t delayMs_ = 0;
};

}  // namespace simpleds::services
