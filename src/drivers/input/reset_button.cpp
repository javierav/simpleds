#include "drivers/input/reset_button.h"

#include <Arduino.h>

namespace simpleds::drivers {

void ResetButton::begin() { pinMode(pin_, INPUT_PULLUP); }

bool ResetButton::update() { return detector_.update(digitalRead(pin_) == LOW, millis()); }

}  // namespace simpleds::drivers
