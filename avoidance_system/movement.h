#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <Arduino.h>
#include <Servo.h>
#include "config.h"

// Position counters for tracking movement
extern int forwardCounter;
extern int backwardCounter;
extern int rightCounter;
extern int leftCounter;

// Servo objects
extern Servo servoChannel3; // Forward/backward control
extern Servo servoChannel4; // Left/right control
extern Servo servoChannel5; // Mode control

// Function declarations
void setupMovement();
void moveDirection(Direction dir, Servo& servo, int pulseValue);
void countedMove(Direction dir, Servo& servo, int pulseValue, int& counter);
void restorePosition(Direction dir, Servo& servo, int pulseValue, int& counter);
void resetCounters();

#endif // MOVEMENT_H 