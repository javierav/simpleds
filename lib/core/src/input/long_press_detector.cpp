#include "input/long_press_detector.h"

namespace simpleds::core {

bool LongPressDetector::update(bool pressed, uint32_t nowMs) {
  if (!pressed) {
    pressed_ = false;
    fired_ = false;
    return false;
  }

  if (!pressed_) {
    pressed_ = true;
    pressedSinceMs_ = nowMs;
  }

  if (!fired_ && nowMs - pressedSinceMs_ >= holdMs_) {
    fired_ = true;
    return true;
  }
  return false;
}

}  // namespace simpleds::core
