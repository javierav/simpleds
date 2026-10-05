#pragma once

#include <esp_timer.h>

#include <atomic>
#include <cstdint>

#include "control/cycle_modulator.h"
#include "output/load_output.h"

namespace simpleds::drivers {

// Zero-crossing SSR driven in whole mains cycles. A periodic esp_timer, one
// tick per cycle, decides with a CycleModulator whether the SSR input stays
// on for the next cycle; the SSR itself switches at the zero crossings.
//
// setLevel() must be called at least every `leaseMs`: otherwise the timer
// switches the load off, so a hung main loop cannot leave it on.
class SsrOutput : public core::ILoadOutput {
 public:
  SsrOutput(uint8_t pin, uint32_t mainsFrequencyHz, uint32_t leaseMs);

  bool begin();
  void setLevel(float level) override;

 private:
  static void onCycle(void* arg);

  uint8_t pin_;
  uint32_t cyclePeriodUs_;
  uint32_t leaseMs_;
  std::atomic<uint16_t> levelPermille_{0};
  std::atomic<uint32_t> refreshedAtMs_{0};
  core::CycleModulator modulator_;  // Only used from the timer task.
  esp_timer_handle_t timer_ = nullptr;
};

}  // namespace simpleds::drivers
