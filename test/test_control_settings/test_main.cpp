#include <unity.h>

#include "settings/control_settings.h"

using simpleds::core::ControlSettings;
using simpleds::core::validateControlSettings;

void setUp() {}
void tearDown() {}

static ControlSettings valid() { return {70.0f, 55.0f, 45.0f, 40.0f, 30, 60}; }

static void assertRejected(const ControlSettings& settings, const char* field) {
  const auto error = validateControlSettings(settings);
  TEST_ASSERT_TRUE_MESSAGE(error.has_value(), field);
  TEST_ASSERT_EQUAL_STRING(field, error->field);
}

static void test_accepts_valid_settings() {
  TEST_ASSERT_FALSE(validateControlSettings(valid()).has_value());
}

static void test_rejects_thresholds_out_of_range() {
  ControlSettings s = valid();
  s.ssrTripC = 101.0f;
  assertRejected(s, "ssrTripC");
  s = valid();
  s.fanOffC = 19.0f;
  assertRejected(s, "fanOffC");
}

static void test_rejects_inverted_hysteresis() {
  ControlSettings s = valid();
  s.ssrResetC = 70.0f;
  assertRejected(s, "ssrResetC");
  s = valid();
  s.fanOffC = 46.0f;
  assertRejected(s, "fanOffC");
}

static void test_fan_must_start_before_trip() {
  ControlSettings s = valid();
  s.fanOnC = 70.0f;
  assertRejected(s, "fanOnC");
}

static void test_rejects_intervals_out_of_range() {
  ControlSettings s = valid();
  s.telemetryIntervalS = 4;
  assertRejected(s, "telemetryIntervalS");
  s = valid();
  s.commandTimeoutS = 3601;
  assertRejected(s, "commandTimeoutS");
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_accepts_valid_settings);
  RUN_TEST(test_rejects_thresholds_out_of_range);
  RUN_TEST(test_rejects_inverted_hysteresis);
  RUN_TEST(test_fan_must_start_before_trip);
  RUN_TEST(test_rejects_intervals_out_of_range);
  return UNITY_END();
}
