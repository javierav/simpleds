#include <unity.h>

#include "control/power_command.h"

using simpleds::core::parsePowerPercent;

void setUp() {}
void tearDown() {}

static void assertParses(const char* payload, float expected) {
  const auto percent = parsePowerPercent(payload);
  TEST_ASSERT_TRUE_MESSAGE(percent.has_value(), payload);
  TEST_ASSERT_EQUAL_FLOAT(expected, *percent);
}

static void test_accepts_valid_percentages() {
  assertParses("0", 0.0f);
  assertParses("100", 100.0f);
  assertParses("42", 42.0f);
  assertParses("42.5", 42.5f);
  assertParses(" 7\n", 7.0f);
  assertParses("007", 7.0f);
  assertParses("3.", 3.0f);
  assertParses(".5", 0.5f);
}

static void test_rejects_out_of_range() {
  TEST_ASSERT_FALSE(parsePowerPercent("100.1").has_value());
  TEST_ASSERT_FALSE(parsePowerPercent("-1").has_value());
  TEST_ASSERT_FALSE(parsePowerPercent("1000").has_value());
}

static void test_rejects_malformed() {
  const char* invalid[] = {"",    "  ",    ".",  "abc",  "50%", "1e2",          "nan",
                           "inf", "1.2.3", "+5", "0x10", "5 5", "{\"power\":5}"};
  for (const char* payload : invalid) {
    TEST_ASSERT_FALSE_MESSAGE(parsePowerPercent(payload).has_value(), payload);
  }
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_accepts_valid_percentages);
  RUN_TEST(test_rejects_out_of_range);
  RUN_TEST(test_rejects_malformed);
  return UNITY_END();
}
