#include "auth/login_throttle.h"

namespace simpleds::core {

bool LoginThrottle::isLocked(uint32_t nowMs) const {
  return failures_ >= maxFailures_ && nowMs - lockedAtMs_ < lockoutMs_;
}

void LoginThrottle::recordFailure(uint32_t nowMs) {
  // A failure after the lockout has expired starts a new round of attempts.
  if (failures_ >= maxFailures_) failures_ = 0;
  if (++failures_ >= maxFailures_) lockedAtMs_ = nowMs;
}

}  // namespace simpleds::core
