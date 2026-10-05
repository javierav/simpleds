#include <unity.h>

#include "control/cycle_modulator.h"

using simpleds::core::CycleModulator;

void setUp() {}
void tearDown() {}

static int countOn(CycleModulator& modulator, uint16_t levelPermille, int cycles) {
  int on = 0;
  for (int i = 0; i < cycles; ++i) on += modulator.nextCycle(levelPermille) ? 1 : 0;
  return on;
}

static void test_zero_never_conducts() {
  CycleModulator modulator;
  TEST_ASSERT_EQUAL(0, countOn(modulator, 0, 1000));
}

static void test_full_always_conducts() {
  CycleModulator modulator;
  TEST_ASSERT_EQUAL(1000, countOn(modulator, 1000, 1000));
  TEST_ASSERT_EQUAL(100, countOn(modulator, 5000, 100));
}

static void test_half_alternates() {
  CycleModulator modulator;
  bool previous = modulator.nextCycle(500);
  for (int i = 0; i < 20; ++i) {
    const bool current = modulator.nextCycle(500);
    TEST_ASSERT_NOT_EQUAL(previous, current);
    previous = current;
  }
}

static void test_quarter_is_one_in_four() {
  CycleModulator modulator;
  for (int block = 0; block < 10; ++block) {
    TEST_ASSERT_EQUAL(1, countOn(modulator, 250, 4));
  }
}

static void test_average_matches_level() {
  for (uint16_t level = 0; level <= 1000; level += 37) {
    CycleModulator modulator;
    TEST_ASSERT_INT_WITHIN(1, level, countOn(modulator, level, 1000));
  }
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_zero_never_conducts);
  RUN_TEST(test_full_always_conducts);
  RUN_TEST(test_half_alternates);
  RUN_TEST(test_quarter_is_one_in_four);
  RUN_TEST(test_average_matches_level);
  return UNITY_END();
}
