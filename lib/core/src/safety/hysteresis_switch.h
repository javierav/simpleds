#pragma once

namespace simpleds::core {

// Two-threshold switch: turns on when the value reaches `onAt` and off when it
// falls to `offAt` (offAt < onAt), so it does not chatter around a threshold.
class HysteresisSwitch {
 public:
  HysteresisSwitch(float onAt, float offAt) : onAt_(onAt), offAt_(offAt) {}

  void setThresholds(float onAt, float offAt);
  bool update(float value);
  bool isOn() const { return on_; }
  // Turns the switch on regardless of the value; it turns off at `offAt`.
  void forceOn() { on_ = true; }

 private:
  float onAt_;
  float offAt_;
  bool on_ = false;
};

}  // namespace simpleds::core
