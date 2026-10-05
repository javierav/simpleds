#pragma once

#include <cstdint>

namespace simpleds::core {

// Detects a button held down continuously for a given time. Fires once per
// press; the button must be released before it can fire again.
class LongPressDetector {
 public:
  explicit LongPressDetector(uint32_t holdMs) : holdMs_(holdMs) {}

  // Feeds the current button state. Returns true only on the call where the
  // hold time is reached.
  bool update(bool pressed, uint32_t nowMs);

 private:
  uint32_t holdMs_;
  uint32_t pressedSinceMs_ = 0;
  bool pressed_ = false;
  bool fired_ = false;
};

}  // namespace simpleds::core
