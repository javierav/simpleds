#include <unity.h>

#include "network/scanned_network.h"

using simpleds::core::prepareNetworkList;
using simpleds::core::ScannedNetwork;
using simpleds::core::signalBars;

void setUp() {}
void tearDown() {}

static void test_sorts_by_signal_strength() {
  const auto list = prepareNetworkList({{"weak", -80, true}, {"strong", -40, true}, {"mid", -60, false}});
  TEST_ASSERT_EQUAL(3, list.size());
  TEST_ASSERT_EQUAL_STRING("strong", list[0].ssid.c_str());
  TEST_ASSERT_EQUAL_STRING("mid", list[1].ssid.c_str());
  TEST_ASSERT_EQUAL_STRING("weak", list[2].ssid.c_str());
}

static void test_keeps_strongest_duplicate() {
  const auto list = prepareNetworkList({{"home", -80, true}, {"home", -50, true}});
  TEST_ASSERT_EQUAL(1, list.size());
  TEST_ASSERT_EQUAL(-50, list[0].rssi);
}

static void test_drops_hidden_networks() {
  const auto list = prepareNetworkList({{"", -40, true}, {"home", -50, true}});
  TEST_ASSERT_EQUAL(1, list.size());
  TEST_ASSERT_EQUAL_STRING("home", list[0].ssid.c_str());
}

static void test_signal_bars() {
  TEST_ASSERT_EQUAL(4, signalBars(-50));
  TEST_ASSERT_EQUAL(3, signalBars(-65));
  TEST_ASSERT_EQUAL(2, signalBars(-70));
  TEST_ASSERT_EQUAL(1, signalBars(-90));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_sorts_by_signal_strength);
  RUN_TEST(test_keeps_strongest_duplicate);
  RUN_TEST(test_drops_hidden_networks);
  RUN_TEST(test_signal_bars);
  return UNITY_END();
}
