#pragma once

#include <cstdint>
#include <optional>

namespace simpleds::core {

// Keeps the last power command received from the controller and expires it
// when no new command arrives in time, so that the load is switched off if
// the controller or the connection to it fails.
class CommandTracker {
 public:
  explicit CommandTracker(uint32_t timeoutMs) : timeoutMs_(timeoutMs) {}

  void setTimeoutMs(uint32_t timeoutMs) { timeoutMs_ = timeoutMs; }

  void accept(float percent, uint32_t nowMs);
  // Drops the current command (invalid payload, connection lost…).
  void clear() { percent_.reset(); }

  // The command in force, or nothing if there is none or it has expired.
  std::optional<float> activePercent(uint32_t nowMs) const;

 private:
  uint32_t timeoutMs_;
  std::optional<float> percent_;
  uint32_t receivedAtMs_ = 0;
};

}  // namespace simpleds::core
