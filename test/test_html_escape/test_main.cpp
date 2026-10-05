#include <unity.h>

#include "text/html_escape.h"

using simpleds::core::escapeHtml;

void setUp() {}
void tearDown() {}

static void test_keeps_plain_text() {
  TEST_ASSERT_EQUAL_STRING("Mi red 5G", escapeHtml("Mi red 5G").c_str());
}

static void test_escapes_special_characters() {
  TEST_ASSERT_EQUAL_STRING("&lt;a href=&quot;x&quot;&gt;Tom&#39;s &amp; co&lt;/a&gt;",
                           escapeHtml("<a href=\"x\">Tom's & co</a>").c_str());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_keeps_plain_text);
  RUN_TEST(test_escapes_special_characters);
  return UNITY_END();
}
