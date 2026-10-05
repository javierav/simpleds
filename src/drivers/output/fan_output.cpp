#include "drivers/output/fan_output.h"

#include <Arduino.h>

namespace simpleds::drivers {

void FanOutput::begin() {
  digitalWrite(pin_, LOW);
  pinMode(pin_, OUTPUT);
}

void FanOutput::set(bool on) {
  if (on == on_) return;
  on_ = on;
  digitalWrite(pin_, on ? HIGH : LOW);
  log_i("Fan %s", on ? "on" : "off");
}

}  // namespace simpleds::drivers
