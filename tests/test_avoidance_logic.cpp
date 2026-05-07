#include <Arduino.h>
#include <Servo.h>
#include "test_framework.h"
#include "../avoidance_system/config.h"
#include "../avoidance_system/sensors.h"
#include "../avoidance_system/movement.h"
#include "../avoidance_system/avoidance_logic.h"

extern bool simulationMode;
extern unsigned long mockMillis;

static SensorData makeData(int f, int b, int l, int r) {
  SensorData d;
  d.frontDistance = f;
  d.backDistance = b;
  d.leftDistance = l;
  d.rightDistance = r;
  d.isValid = true;
  return d;
}

static void clearServos() {
  servoChannel3.lastPulse = -1;
  servoChannel4.lastPulse = -1;
}

void test_avoidance_logic() {
  std::cout << "[avoidance_logic - manual]\n";
  simulationMode = false; // ensure all servo writes happen
  resetAvoidanceState();

  // 1. All clear → RC pass-through on both channels
  clearServos();
  handleObstacleAvoidance(makeData(0, 0, 0, 0), MODE_MANUAL, 1600, 1400);
  REQUIRE(servoChannel3.lastPulse == 1600);
  REQUIRE(servoChannel4.lastPulse == 1400);

  // 2. Front blocked → reverse on ch3
  clearServos();
  handleObstacleAvoidance(makeData(20, 0, 0, 0), MODE_MANUAL, 1500, 1500);
  REQUIRE(servoChannel3.lastPulse == BACKWARD_PULSE);

  // 3. Back blocked → forward on ch3
  clearServos();
  handleObstacleAvoidance(makeData(0, 20, 0, 0), MODE_MANUAL, 1500, 1500);
  REQUIRE(servoChannel3.lastPulse == FORWARD_PULSE);

  // 4. Left blocked → strafe right on ch4
  clearServos();
  handleObstacleAvoidance(makeData(0, 0, 20, 0), MODE_MANUAL, 1500, 1500);
  REQUIRE(servoChannel4.lastPulse == FORWARD_PULSE);

  // 5. Right blocked → strafe left on ch4
  clearServos();
  handleObstacleAvoidance(makeData(0, 0, 0, 20), MODE_MANUAL, 1500, 1500);
  REQUIRE(servoChannel4.lastPulse == BACKWARD_PULSE);

  // 6. All four blocked → both channels neutral (early return)
  clearServos();
  handleObstacleAvoidance(makeData(20, 20, 20, 20), MODE_MANUAL, 1500, 1500);
  REQUIRE(servoChannel3.lastPulse == NEUTRAL_PULSE);
  REQUIRE(servoChannel4.lastPulse == NEUTRAL_PULSE);

  // 7. Front+back blocked, left blocked, right open → ch3 neutral, ch4 strafe right
  clearServos();
  handleObstacleAvoidance(makeData(20, 20, 20, 0), MODE_MANUAL, 1500, 1500);
  REQUIRE(servoChannel3.lastPulse == NEUTRAL_PULSE);
  REQUIRE(servoChannel4.lastPulse == FORWARD_PULSE);

  // 8. Front+back blocked, right blocked, left open → ch3 neutral, ch4 strafe left
  clearServos();
  handleObstacleAvoidance(makeData(20, 20, 0, 20), MODE_MANUAL, 1500, 1500);
  REQUIRE(servoChannel3.lastPulse == NEUTRAL_PULSE);
  REQUIRE(servoChannel4.lastPulse == BACKWARD_PULSE);

  // 9. Front blocked + left blocked → ch3 reverse, ch4 strafe right
  clearServos();
  handleObstacleAvoidance(makeData(20, 0, 20, 0), MODE_MANUAL, 1500, 1500);
  REQUIRE(servoChannel3.lastPulse == BACKWARD_PULSE);
  REQUIRE(servoChannel4.lastPulse == FORWARD_PULSE);

  // 10. Loiter mode mirrors Manual — front blocked → ch3 reverse
  clearServos();
  handleObstacleAvoidance(makeData(20, 0, 0, 0), MODE_LOITER, 1500, 1500);
  REQUIRE(servoChannel3.lastPulse == BACKWARD_PULSE);
}

void test_position_hold() {
  std::cout << "[avoidance_logic - position hold]\n";
  simulationMode = false;
  resetAvoidanceState();

  // Front blocks at t=1000 → start backward avoidance
  mockMillis = 1000;
  clearServos();
  handleObstacleAvoidance(makeData(20, 0, 0, 0), MODE_POSITION_HOLD, 1500, 1500);
  REQUIRE(servoChannel3.lastPulse == BACKWARD_PULSE);
  REQUIRE(avoidState.forwardAvoidActive == true);
  REQUIRE(avoidState.forwardStartTime == 1000);

  // Still blocked at t=3000 → duration = 2000
  mockMillis = 3000;
  handleObstacleAvoidance(makeData(20, 0, 0, 0), MODE_POSITION_HOLD, 1500, 1500);
  REQUIRE(avoidState.forwardDuration == 2000);

  // Obstacle clears at t=4000 → restore phase begins
  mockMillis = 4000;
  clearServos();
  handleObstacleAvoidance(makeData(0, 0, 0, 0), MODE_POSITION_HOLD, 1500, 1500);
  REQUIRE(servoChannel3.lastPulse == FORWARD_PULSE); // restoring forward
  REQUIRE(avoidState.forwardAvoidActive == false);

  // After 2000ms of restore (t=6001) → restore completes, neutral, state cleared
  mockMillis = 6001;
  clearServos();
  handleObstacleAvoidance(makeData(0, 0, 0, 0), MODE_POSITION_HOLD, 1500, 1500);
  REQUIRE(servoChannel3.lastPulse == NEUTRAL_PULSE);
  REQUIRE(avoidState.forwardDuration == 0);

  // Long block must be capped: at t=20000 (already capped), restore takes only MAX_AVOID_MS
  resetAvoidanceState();
  mockMillis = 10000;
  handleObstacleAvoidance(makeData(20, 0, 0, 0), MODE_POSITION_HOLD, 1500, 1500);
  mockMillis = 30000; // 20s blocked
  handleObstacleAvoidance(makeData(20, 0, 0, 0), MODE_POSITION_HOLD, 1500, 1500);
  REQUIRE(avoidState.forwardDuration == MAX_AVOID_MS);
}
