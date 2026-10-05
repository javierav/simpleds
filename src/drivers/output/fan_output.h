#pragma once

#include <cstdint>

namespace simpleds::drivers {

// On/off fan driven through a transistor (active high).
class FanOutput {
 public:
  explicit FanOutput(uint8_t pin) : pin_(pin) {}

  void begin();
  void set(bool on);
  bool isOn() const { return on_; }

 private:
  uint8_t pin_;
  bool on_ = false;
};

}  // namespace simpleds::drivers
