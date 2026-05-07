#include <Arduino.h>
#include <Servo.h>
#include "test_framework.h"
#include "../avoidance_system/config.h"
#include "../avoidance_system/sensors.h"
#include "../avoidance_system/movement.h"

extern unsigned long mockMillis;
extern bool simulationMode;

void test_movement() {
  std::cout << "[movement]\n";
  simulationMode = true; // skip writeMicroseconds; assert duration logic only

  unsigned long startTime = 0;
  unsigned long duration = 0;
  Servo dummy;

  // First call: startTime gets stamped, duration starts at 0
  mockMillis = 1000;
  startTimedMove(BACKWARD, dummy, BACKWARD_PULSE, startTime, duration);
  REQUIRE(startTime == 1000);
  REQUIRE(duration == 0);

  // 2s later: duration accumulates
  mockMillis = 3000;
  startTimedMove(BACKWARD, dummy, BACKWARD_PULSE, startTime, duration);
  REQUIRE(duration == 2000);

  // After 10s elapsed, duration must be clamped at MAX_AVOID_MS
  mockMillis = 11000;
  startTimedMove(BACKWARD, dummy, BACKWARD_PULSE, startTime, duration);
  REQUIRE(duration == MAX_AVOID_MS);

  // Restore phase: should run for `duration`, then complete
  unsigned long restoreStart = 0;
  mockMillis = 20000;
  bool ongoing = doRestorePosition(FORWARD, dummy, FORWARD_PULSE, restoreStart, duration);
  REQUIRE(ongoing == true);
  REQUIRE(restoreStart == 20000);

  // Just before duration elapses
  mockMillis = 20000 + MAX_AVOID_MS - 1;
  ongoing = doRestorePosition(FORWARD, dummy, FORWARD_PULSE, restoreStart, duration);
  REQUIRE(ongoing == true);

  // At/past the boundary: restore completes and resets state
  mockMillis = 20000 + MAX_AVOID_MS;
  ongoing = doRestorePosition(FORWARD, dummy, FORWARD_PULSE, restoreStart, duration);
  REQUIRE(ongoing == false);
  REQUIRE(duration == 0);
  REQUIRE(restoreStart == 0);

  // doRestorePosition with zero duration is a no-op
  unsigned long zStart = 0, zDuration = 0;
  ongoing = doRestorePosition(FORWARD, dummy, FORWARD_PULSE, zStart, zDuration);
  REQUIRE(ongoing == false);
}
