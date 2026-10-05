#include "safety/hysteresis_switch.h"

namespace simpleds::core {

void HysteresisSwitch::setThresholds(float onAt, float offAt) {
  onAt_ = onAt;
  offAt_ = offAt;
}

bool HysteresisSwitch::update(float value) {
  if (value >= onAt_) {
    on_ = true;
  } else if (value <= offAt_) {
    on_ = false;
  }
  return on_;
}

}  // namespace simpleds::core
