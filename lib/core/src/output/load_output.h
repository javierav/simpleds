#pragma once

namespace simpleds::core {

// Applies a normalized power level to the resistive load. Hides how the power
// is modulated (SSR bursts, phase angle, external regulator…).
class ILoadOutput {
 public:
  virtual ~ILoadOutput() = default;

  // level: 0.0 (off) … 1.0 (full power). Values outside the range are clamped.
  virtual void setLevel(float level) = 0;
};

}  // namespace simpleds::core
