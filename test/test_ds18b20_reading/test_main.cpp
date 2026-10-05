#include <unity.h>

#include <cmath>

#include "sensors/ds18b20_reading.h"

using simpleds::core::validateDs18b20Reading;

void setUp() {}
void tearDown() {}

static void test_accepts_plausible_values() {
  TEST_ASSERT_EQUAL_FLOAT(21.5f, *validateDs18b20Reading(21.5f));
  TEST_ASSERT_EQUAL_FLOAT(-55.0f, *validateDs18b20Reading(-55.0f));
  TEST_ASSERT_EQUAL_FLOAT(125.0f, *validateDs18b20Reading(125.0f));
  TEST_ASSERT_EQUAL_FLOAT(84.9375f, *validateDs18b20Reading(84.9375f));
}

static void test_rejects_error_markers() {
  TEST_ASSERT_FALSE(validateDs18b20Reading(-127.0f).has_value());
  TEST_ASSERT_FALSE(validateDs18b20Reading(85.0f).has_value());
  TEST_ASSERT_FALSE(validateDs18b20Reading(125.5f).has_value());
  TEST_ASSERT_FALSE(validateDs18b20Reading(NAN).has_value());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_accepts_plausible_values);
  RUN_TEST(test_rejects_error_markers);
  return UNITY_END();
}
