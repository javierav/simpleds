#include <unity.h>

#include <cstdint>
#include <string>

#include "auth/cookie.h"
#include "auth/login_throttle.h"
#include "auth/panel_password.h"

using namespace simpleds::core;

void setUp() {}
void tearDown() {}

static void test_password_rules() {
  TEST_ASSERT_FALSE(validatePanelPassword("").has_value());
  TEST_ASSERT_FALSE(validatePanelPassword("12345678").has_value());
  TEST_ASSERT_EQUAL_STRING("too_short", validatePanelPassword("1234567")->code);
  TEST_ASSERT_EQUAL_STRING("too_long", validatePanelPassword(std::string(65, 'x'))->code);
}

static void test_constant_time_equals() {
  TEST_ASSERT_TRUE(constantTimeEquals("secret", "secret"));
  TEST_ASSERT_TRUE(constantTimeEquals("", ""));
  TEST_ASSERT_FALSE(constantTimeEquals("secret", "secreT"));
  TEST_ASSERT_FALSE(constantTimeEquals("secret", "secrets"));
  TEST_ASSERT_FALSE(constantTimeEquals("", "x"));
}

static void test_find_cookie() {
  TEST_ASSERT_EQUAL_STRING("abc", std::string(findCookie("sid=abc", "sid")).c_str());
  TEST_ASSERT_EQUAL_STRING("abc", std::string(findCookie("a=1; sid=abc; b=2", "sid")).c_str());
  TEST_ASSERT_TRUE(findCookie("xsid=abc; a=1", "sid").empty());
  TEST_ASSERT_TRUE(findCookie("", "sid").empty());
  TEST_ASSERT_TRUE(findCookie("sid", "sid").empty());
}

static void test_throttle_locks_after_failures() {
  LoginThrottle throttle(3, 1000);
  throttle.recordFailure(0);
  throttle.recordFailure(10);
  TEST_ASSERT_FALSE(throttle.isLocked(20));
  throttle.recordFailure(100);
  TEST_ASSERT_TRUE(throttle.isLocked(1099));
  TEST_ASSERT_FALSE(throttle.isLocked(1100));
  // After the lockout a new round of attempts starts.
  throttle.recordFailure(1200);
  TEST_ASSERT_FALSE(throttle.isLocked(1201));
}

static void test_throttle_success_resets() {
  LoginThrottle throttle(3, 1000);
  throttle.recordFailure(0);
  throttle.recordFailure(0);
  throttle.recordSuccess();
  throttle.recordFailure(0);
  TEST_ASSERT_FALSE(throttle.isLocked(1));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_password_rules);
  RUN_TEST(test_constant_time_equals);
  RUN_TEST(test_find_cookie);
  RUN_TEST(test_throttle_locks_after_failures);
  RUN_TEST(test_throttle_success_resets);
  return UNITY_END();
}
