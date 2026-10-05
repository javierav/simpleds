#include <unity.h>

#include "text/number_format.h"

using simpleds::core::formatOneDecimal;

void setUp() {}
void tearDown() {}

static void test_formats_one_decimal() {
  TEST_ASSERT_EQUAL_STRING("54.3", formatOneDecimal(54.3125f).c_str());
  TEST_ASSERT_EQUAL_STRING("100.0", formatOneDecimal(100.0f).c_str());
  TEST_ASSERT_EQUAL_STRING("-2.5", formatOneDecimal(-2.5f).c_str());
}

static void test_no_negative_zero() {
  TEST_ASSERT_EQUAL_STRING("0.0", formatOneDecimal(-0.04f).c_str());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_formats_one_decimal);
  RUN_TEST(test_no_negative_zero);
  return UNITY_END();
}
