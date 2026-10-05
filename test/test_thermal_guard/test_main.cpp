#include <unity.h>

#include <optional>

#include "safety/thermal_guard.h"

using simpleds::core::EmergencyReason;
using simpleds::core::ThermalGuard;
using simpleds::core::ThermalState;

void setUp() {}
void tearDown() {}

static ThermalGuard makeGuard() { return ThermalGuard(70.0f, 55.0f, 3); }

static void test_starts_without_output() {
  ThermalGuard guard = makeGuard();
  TEST_ASSERT_TRUE(guard.state() == ThermalState::kStarting);
  TEST_ASSERT_FALSE(guard.allowsOutput());
}

static void test_active_with_normal_temperature() {
  ThermalGuard guard = makeGuard();
  guard.update(40.0f);
  TEST_ASSERT_TRUE(guard.state() == ThermalState::kActive);
  TEST_ASSERT_TRUE(guard.allowsOutput());
}

static void test_overheat_trips_and_resets_with_hysteresis() {
  ThermalGuard guard = makeGuard();
  guard.update(69.9f);
  TEST_ASSERT_TRUE(guard.allowsOutput());
  guard.update(70.0f);
  TEST_ASSERT_TRUE(guard.state() == ThermalState::kEmergency);
  TEST_ASSERT_TRUE(guard.reason() == EmergencyReason::kOverheat);
  guard.update(60.0f);
  TEST_ASSERT_FALSE(guard.allowsOutput());
  guard.update(55.0f);
  TEST_ASSERT_TRUE(guard.allowsOutput());
  TEST_ASSERT_TRUE(guard.reason() == EmergencyReason::kNone);
}

static void test_isolated_probe_errors_are_tolerated() {
  ThermalGuard guard = makeGuard();
  guard.update(40.0f);
  guard.update(std::nullopt);
  guard.update(std::nullopt);
  TEST_ASSERT_TRUE(guard.allowsOutput());
  guard.update(41.0f);
  guard.update(std::nullopt);
  guard.update(std::nullopt);
  TEST_ASSERT_TRUE(guard.allowsOutput());
}

static void test_repeated_probe_errors_trigger_emergency() {
  ThermalGuard guard = makeGuard();
  guard.update(40.0f);
  for (int i = 0; i < 3; ++i) guard.update(std::nullopt);
  TEST_ASSERT_TRUE(guard.state() == ThermalState::kEmergency);
  TEST_ASSERT_TRUE(guard.reason() == EmergencyReason::kSensorFailure);
}

static void test_missing_probe_at_boot_triggers_emergency() {
  ThermalGuard guard = makeGuard();
  for (int i = 0; i < 3; ++i) guard.update(std::nullopt);
  TEST_ASSERT_TRUE(guard.state() == ThermalState::kEmergency);
}

static void test_recovers_from_probe_failure_only_below_reset() {
  ThermalGuard guard = makeGuard();
  for (int i = 0; i < 3; ++i) guard.update(std::nullopt);
  guard.update(60.0f);
  TEST_ASSERT_FALSE(guard.allowsOutput());
  guard.update(50.0f);
  TEST_ASSERT_TRUE(guard.allowsOutput());
}

static void test_new_thresholds_apply() {
  ThermalGuard guard = makeGuard();
  guard.setThresholds(50.0f, 40.0f);
  guard.update(50.0f);
  TEST_ASSERT_TRUE(guard.state() == ThermalState::kEmergency);
}

static void test_codes() {
  TEST_ASSERT_EQUAL_STRING("active", simpleds::core::stateCode(ThermalState::kActive));
  TEST_ASSERT_EQUAL_STRING("emergency", simpleds::core::stateCode(ThermalState::kEmergency));
  TEST_ASSERT_EQUAL_STRING("overheat", simpleds::core::reasonCode(EmergencyReason::kOverheat));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_starts_without_output);
  RUN_TEST(test_active_with_normal_temperature);
  RUN_TEST(test_overheat_trips_and_resets_with_hysteresis);
  RUN_TEST(test_isolated_probe_errors_are_tolerated);
  RUN_TEST(test_repeated_probe_errors_trigger_emergency);
  RUN_TEST(test_missing_probe_at_boot_triggers_emergency);
  RUN_TEST(test_recovers_from_probe_failure_only_below_reset);
  RUN_TEST(test_new_thresholds_apply);
  RUN_TEST(test_codes);
  return UNITY_END();
}
