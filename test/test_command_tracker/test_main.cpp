#include <unity.h>

#include <cstdint>

#include "control/command_tracker.h"

using simpleds::core::CommandTracker;

void setUp() {}
void tearDown() {}

static void test_no_command_initially() {
  CommandTracker tracker(1000);
  TEST_ASSERT_FALSE(tracker.activePercent(0).has_value());
}

static void test_command_active_until_timeout() {
  CommandTracker tracker(1000);
  tracker.accept(40.0f, 100);
  TEST_ASSERT_EQUAL_FLOAT(40.0f, *tracker.activePercent(1099));
  TEST_ASSERT_FALSE(tracker.activePercent(1100).has_value());
}

static void test_new_command_restarts_timeout() {
  CommandTracker tracker(1000);
  tracker.accept(40.0f, 0);
  tracker.accept(60.0f, 900);
  TEST_ASSERT_EQUAL_FLOAT(60.0f, *tracker.activePercent(1800));
}

static void test_clear_drops_command() {
  CommandTracker tracker(1000);
  tracker.accept(40.0f, 0);
  tracker.clear();
  TEST_ASSERT_FALSE(tracker.activePercent(1).has_value());
}

static void test_handles_millis_overflow() {
  CommandTracker tracker(1000);
  tracker.accept(40.0f, UINT32_MAX - 100);
  TEST_ASSERT_TRUE(tracker.activePercent(800).has_value());
  TEST_ASSERT_FALSE(tracker.activePercent(900).has_value());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_no_command_initially);
  RUN_TEST(test_command_active_until_timeout);
  RUN_TEST(test_new_command_restarts_timeout);
  RUN_TEST(test_clear_drops_command);
  RUN_TEST(test_handles_millis_overflow);
  return UNITY_END();
}
