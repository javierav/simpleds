#include <unity.h>

#include "network/wifi_credentials.h"

using simpleds::core::CredentialsError;
using simpleds::core::errorCode;
using simpleds::core::validateCredentials;
using simpleds::core::WifiCredentials;

void setUp() {}
void tearDown() {}

static void test_accepts_secured_network() {
  TEST_ASSERT_TRUE(validateCredentials({"home", "12345678"}) == CredentialsError::kNone);
}

static void test_accepts_open_network() {
  TEST_ASSERT_TRUE(validateCredentials({"home", ""}) == CredentialsError::kNone);
}

static void test_rejects_empty_ssid() {
  TEST_ASSERT_TRUE(validateCredentials({"", "12345678"}) == CredentialsError::kSsidEmpty);
}

static void test_ssid_length_limit() {
  TEST_ASSERT_TRUE(validateCredentials({std::string(32, 'a'), ""}) == CredentialsError::kNone);
  TEST_ASSERT_TRUE(validateCredentials({std::string(33, 'a'), ""}) ==
                   CredentialsError::kSsidTooLong);
}

static void test_password_length_limits() {
  TEST_ASSERT_TRUE(validateCredentials({"home", "1234567"}) == CredentialsError::kPasswordTooShort);
  TEST_ASSERT_TRUE(validateCredentials({"home", std::string(63, 'p')}) == CredentialsError::kNone);
  TEST_ASSERT_TRUE(validateCredentials({"home", std::string(64, 'p')}) ==
                   CredentialsError::kPasswordTooLong);
}

static void test_error_codes() {
  TEST_ASSERT_EQUAL_STRING("ssid_empty", errorCode(CredentialsError::kSsidEmpty));
  TEST_ASSERT_EQUAL_STRING("password_too_short", errorCode(CredentialsError::kPasswordTooShort));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_accepts_secured_network);
  RUN_TEST(test_accepts_open_network);
  RUN_TEST(test_rejects_empty_ssid);
  RUN_TEST(test_ssid_length_limit);
  RUN_TEST(test_password_length_limits);
  RUN_TEST(test_error_codes);
  return UNITY_END();
}
