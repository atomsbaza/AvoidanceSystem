#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <Arduino.h>
#include <Servo.h>
#include "config.h"

// Time-based avoidance state
struct AvoidanceState {
  unsigned long forwardStartTime;    // millis() at start of current forward-avoidance phase
  unsigned long backwardStartTime;
  unsigned long rightStartTime;
  unsigned long leftStartTime;
  unsigned long forwardDuration;     // accumulated avoidance time (ms)
  unsigned long backwardDuration;
  unsigned long rightDuration;
  unsigned long leftDuration;
  bool forwardAvoidActive;           // true = avoidance in progress, false = restore/idle
  bool backwardAvoidActive;
  bool rightAvoidActive;
  bool leftAvoidActive;
};
extern AvoidanceState avoidState;

// Servo objects
extern Servo servoChannel3; // Forward/backward control
extern Servo servoChannel4; // Left/right control
extern Servo servoChannel5; // Mode control

// Function declarations
void setupMovement();
void moveDirection(Direction dir, Servo& servo, int pulseValue);
void startTimedMove(Direction dir, Servo& servo, int pulseValue,
                    unsigned long& startTime, unsigned long& duration);
bool doRestorePosition(Direction dir, Servo& servo, int restorePulse,
                       unsigned long& startTime, unsigned long& duration);
void resetAvoidanceState();

#endif // MOVEMENT_H
