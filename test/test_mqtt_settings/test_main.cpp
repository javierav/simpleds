#include <unity.h>

#include <string>

#include "settings/mqtt_settings.h"

using simpleds::core::isValidTopic;
using simpleds::core::MqttSettings;
using simpleds::core::validateMqttSettings;

void setUp() {}
void tearDown() {}

static MqttSettings valid() {
  return {"192.168.1.10",
          1883,
          "user",
          "secret",
          "simpleds",
          {"simpleds/in/power", "simpleds/out/power", "simpleds/out/state",
           "simpleds/out/ssr/temperature", "simpleds/out/tank/temperature", "simpleds/out/fan"}};
}

static void assertRejected(const MqttSettings& settings, const char* field, const char* code) {
  const auto error = validateMqttSettings(settings);
  TEST_ASSERT_TRUE_MESSAGE(error.has_value(), field);
  TEST_ASSERT_EQUAL_STRING(field, error->field);
  TEST_ASSERT_EQUAL_STRING(code, error->code);
}

static void test_accepts_valid_settings() {
  TEST_ASSERT_FALSE(validateMqttSettings(valid()).has_value());
  MqttSettings disabled = valid();
  disabled.host.clear();
  TEST_ASSERT_FALSE(validateMqttSettings(disabled).has_value());
  TEST_ASSERT_FALSE(simpleds::core::isMqttEnabled(disabled));
}

static void test_topic_rules() {
  TEST_ASSERT_TRUE(isValidTopic("a/b/c"));
  TEST_ASSERT_TRUE(isValidTopic("home/termo agua"));
  TEST_ASSERT_FALSE(isValidTopic(""));
  TEST_ASSERT_FALSE(isValidTopic("a/+/c"));
  TEST_ASSERT_FALSE(isValidTopic("a/#"));
  TEST_ASSERT_FALSE(isValidTopic(std::string(129, 'a')));
}

static void test_rejects_bad_connection_fields() {
  MqttSettings s = valid();
  s.host = "bad host";
  assertRejected(s, "host", "invalid");
  s = valid();
  s.port = 0;
  assertRejected(s, "port", "out_of_range");
  s = valid();
  s.clientId = "";
  assertRejected(s, "clientId", "invalid");
  s = valid();
  s.password = std::string(65, 'x');
  assertRejected(s, "password", "too_long");
}

static void test_rejects_bad_topics() {
  MqttSettings s = valid();
  s.topics.command = "simpleds/#";
  assertRejected(s, "commandTopic", "invalid");
  s = valid();
  s.topics.state = s.topics.command;
  assertRejected(s, "stateTopic", "same_as_command");
  s = valid();
  s.topics.fan = s.topics.power;
  assertRejected(s, "fanTopic", "duplicated");
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_accepts_valid_settings);
  RUN_TEST(test_topic_rules);
  RUN_TEST(test_rejects_bad_connection_fields);
  RUN_TEST(test_rejects_bad_topics);
  return UNITY_END();
}
