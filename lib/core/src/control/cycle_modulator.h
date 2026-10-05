#pragma once

#include <cstdint>

namespace simpleds::core {

inline constexpr uint16_t kFullLevelPermille = 1000;

// Spreads a power level over whole mains cycles (first-order sigma-delta, as
// in Bresenham's line algorithm): each call decides whether the next cycle
// conducts, so that the ratio of conducting cycles tracks the level and they
// are as evenly distributed as possible (e.g. 50 % = on, off, on, off…).
class CycleModulator {
 public:
  // levelPermille: 0 … 1000; higher values are treated as 1000.
  bool nextCycle(uint16_t levelPermille);
  void reset() { accumulator_ = 0; }

 private:
  uint16_t accumulator_ = 0;
};

}  // namespace simpleds::core
