#include <unity.h>

#include "network/connection_failure.h"

using simpleds::core::classifyDisconnectReason;
using simpleds::core::ConnectionFailure;
using simpleds::core::failureCode;

void setUp() {}
void tearDown() {}

static void test_wrong_password_reasons() {
  TEST_ASSERT_TRUE(classifyDisconnectReason(15) == ConnectionFailure::kWrongPassword);
  TEST_ASSERT_TRUE(classifyDisconnectReason(202) == ConnectionFailure::kWrongPassword);
  TEST_ASSERT_TRUE(classifyDisconnectReason(204) == ConnectionFailure::kWrongPassword);
}

static void test_network_not_found_reason() {
  TEST_ASSERT_TRUE(classifyDisconnectReason(201) == ConnectionFailure::kNetworkNotFound);
}

static void test_other_reasons() {
  TEST_ASSERT_TRUE(classifyDisconnectReason(2) == ConnectionFailure::kOther);
  TEST_ASSERT_TRUE(classifyDisconnectReason(205) == ConnectionFailure::kOther);
}

static void test_failure_codes() {
  TEST_ASSERT_EQUAL_STRING("wrong_password", failureCode(ConnectionFailure::kWrongPassword));
  TEST_ASSERT_EQUAL_STRING("network_not_found", failureCode(ConnectionFailure::kNetworkNotFound));
  TEST_ASSERT_EQUAL_STRING("timeout", failureCode(ConnectionFailure::kTimeout));
  TEST_ASSERT_EQUAL_STRING("connection_failed", failureCode(ConnectionFailure::kOther));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_wrong_password_reasons);
  RUN_TEST(test_network_not_found_reason);
  RUN_TEST(test_other_reasons);
  RUN_TEST(test_failure_codes);
  return UNITY_END();
}
