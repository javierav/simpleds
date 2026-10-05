#include "control/command_tracker.h"

namespace simpleds::core {

void CommandTracker::accept(float percent, uint32_t nowMs) {
  percent_ = percent;
  receivedAtMs_ = nowMs;
}

std::optional<float> CommandTracker::activePercent(uint32_t nowMs) const {
  if (!percent_ || nowMs - receivedAtMs_ >= timeoutMs_) return std::nullopt;
  return percent_;
}

}  // namespace simpleds::core
