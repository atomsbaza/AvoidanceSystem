#include <Arduino.h>
#include "test_framework.h"

int testsRun = 0;
int testsFailed = 0;
unsigned long mockMillis = 0;
SerialStub Serial;

void test_sensors();
void test_movement();
void test_avoidance_logic();
void test_position_hold();

int main() {
  std::cout << "Running tests...\n";
  test_sensors();
  test_movement();
  test_avoidance_logic();
  test_position_hold();

  int passed = testsRun - testsFailed;
  std::cout << "\n" << passed << "/" << testsRun << " passed";
  if (testsFailed > 0) {
    std::cout << ", " << testsFailed << " FAILED\n";
    return 1;
  }
  std::cout << "\n";
  return 0;
}
