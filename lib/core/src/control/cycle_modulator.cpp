#include "control/cycle_modulator.h"

namespace simpleds::core {

bool CycleModulator::nextCycle(uint16_t levelPermille) {
  if (levelPermille >= kFullLevelPermille) return true;
  accumulator_ += levelPermille;
  if (accumulator_ < kFullLevelPermille) return false;
  accumulator_ -= kFullLevelPermille;
  return true;
}

}  // namespace simpleds::core
