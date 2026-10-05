#include <unity.h>

#include <cstdint>

#include "input/long_press_detector.h"

using simpleds::core::LongPressDetector;

void setUp() {}
void tearDown() {}

static void test_does_not_fire_when_released() {
  LongPressDetector detector(1000);
  TEST_ASSERT_FALSE(detector.update(false, 0));
  TEST_ASSERT_FALSE(detector.update(false, 5000));
}

static void test_fires_once_after_hold_time() {
  LongPressDetector detector(1000);
  TEST_ASSERT_FALSE(detector.update(true, 100));
  TEST_ASSERT_FALSE(detector.update(true, 1099));
  TEST_ASSERT_TRUE(detector.update(true, 1100));
  TEST_ASSERT_FALSE(detector.update(true, 3000));
}

static void test_short_press_resets() {
  LongPressDetector detector(1000);
  detector.update(true, 0);
  detector.update(false, 900);
  TEST_ASSERT_FALSE(detector.update(true, 1000));
  TEST_ASSERT_FALSE(detector.update(true, 1999));
  TEST_ASSERT_TRUE(detector.update(true, 2000));
}

static void test_fires_again_after_release() {
  LongPressDetector detector(1000);
  detector.update(true, 0);
  TEST_ASSERT_TRUE(detector.update(true, 1000));
  detector.update(false, 1500);
  detector.update(true, 2000);
  TEST_ASSERT_TRUE(detector.update(true, 3000));
}

static void test_handles_millis_overflow() {
  LongPressDetector detector(1000);
  detector.update(true, UINT32_MAX - 100);
  TEST_ASSERT_FALSE(detector.update(true, 800));
  TEST_ASSERT_TRUE(detector.update(true, 900));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_does_not_fire_when_released);
  RUN_TEST(test_fires_once_after_hold_time);
  RUN_TEST(test_short_press_resets);
  RUN_TEST(test_fires_again_after_release);
  RUN_TEST(test_handles_millis_overflow);
  return UNITY_END();
}
