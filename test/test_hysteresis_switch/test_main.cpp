#include <unity.h>

#include "safety/hysteresis_switch.h"

using simpleds::core::HysteresisSwitch;

void setUp() {}
void tearDown() {}

static void test_switches_with_hysteresis() {
  HysteresisSwitch fan(45.0f, 40.0f);
  TEST_ASSERT_FALSE(fan.update(44.9f));
  TEST_ASSERT_TRUE(fan.update(45.0f));
  TEST_ASSERT_TRUE(fan.update(42.0f));
  TEST_ASSERT_TRUE(fan.update(40.1f));
  TEST_ASSERT_FALSE(fan.update(40.0f));
  TEST_ASSERT_FALSE(fan.update(44.0f));
}

static void test_force_on_holds_until_off_threshold() {
  HysteresisSwitch sw(45.0f, 40.0f);
  sw.forceOn();
  TEST_ASSERT_TRUE(sw.update(42.0f));
  TEST_ASSERT_FALSE(sw.update(39.0f));
}

static void test_new_thresholds_apply() {
  HysteresisSwitch sw(45.0f, 40.0f);
  sw.setThresholds(30.0f, 25.0f);
  TEST_ASSERT_TRUE(sw.update(30.0f));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_switches_with_hysteresis);
  RUN_TEST(test_force_on_holds_until_off_threshold);
  RUN_TEST(test_new_thresholds_apply);
  return UNITY_END();
}
