#pragma once

#include <cstdint>

namespace simpleds::core {

// Slows down password guessing: after `maxFailures` consecutive failed logins,
// further attempts are rejected for `lockoutMs`.
class LoginThrottle {
 public:
  LoginThrottle(uint8_t maxFailures, uint32_t lockoutMs)
      : maxFailures_(maxFailures), lockoutMs_(lockoutMs) {}

  bool isLocked(uint32_t nowMs) const;
  void recordFailure(uint32_t nowMs);
  void recordSuccess() { failures_ = 0; }

 private:
  uint8_t maxFailures_;
  uint32_t lockoutMs_;
  uint8_t failures_ = 0;
  uint32_t lockedAtMs_ = 0;
};

}  // namespace simpleds::core
