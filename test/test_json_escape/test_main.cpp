#include <unity.h>

#include "text/json_escape.h"

using simpleds::core::escapeJson;

void setUp() {}
void tearDown() {}

static void test_keeps_plain_text() {
  TEST_ASSERT_EQUAL_STRING("Mi red 5G ñ", escapeJson("Mi red 5G ñ").c_str());
}

static void test_escapes_quotes_and_backslashes() {
  TEST_ASSERT_EQUAL_STRING("a\\\"b\\\\c", escapeJson("a\"b\\c").c_str());
}

static void test_escapes_control_characters() {
  TEST_ASSERT_EQUAL_STRING("a\\nb\\tc\\u0001", escapeJson("a\nb\tc\x01").c_str());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_keeps_plain_text);
  RUN_TEST(test_escapes_quotes_and_backslashes);
  RUN_TEST(test_escapes_control_characters);
  return UNITY_END();
}
