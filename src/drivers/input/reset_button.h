#pragma once

#include <cstdint>

#include "input/long_press_detector.h"

namespace simpleds::drivers {

// Active-low push button that reports long presses.
class ResetButton {
 public:
  ResetButton(uint8_t pin, uint32_t holdMs) : pin_(pin), detector_(holdMs) {}

  void begin();

  // Returns true once when the button has been held for the hold time.
  bool update();

 private:
  uint8_t pin_;
  core::LongPressDetector detector_;
};

}  // namespace simpleds::drivers
