#include <Arduino.h>
#include "test_framework.h"
#include "../avoidance_system/config.h"
#include "../avoidance_system/sensors.h"

void test_sensors() {
  std::cout << "[sensors]\n";
  REQUIRE(validateSensorReading(0) == true);                  // no obstacle
  REQUIRE(validateSensorReading(1) == false);                 // below 2cm — invalid
  REQUIRE(validateSensorReading(2) == true);                  // min boundary
  REQUIRE(validateSensorReading(MAX_DISTANCE) == true);       // max boundary
  REQUIRE(validateSensorReading(MAX_DISTANCE + 1) == false);  // beyond max
  REQUIRE(validateSensorReading(-1) == false);                // negative — invalid
}
