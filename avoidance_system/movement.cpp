#include "movement.h"
#include "sensors.h"

// Avoidance state (time-based tracking)
AvoidanceState avoidState = {0};

// Servo objects for controlling ESCs and servos
Servo servoChannel3; // Forward/backward control
Servo servoChannel4; // Left/right control
Servo servoChannel5; // Mode control

// Initialize servos and attach them to pins
void setupMovement() {
  if (!simulationMode) {
    servoChannel3.attach(CHANNEL3_OUT_PIN);
    servoChannel4.attach(CHANNEL4_OUT_PIN);
    servoChannel5.attach(CHANNEL5_OUT_PIN);
  } else {
    Serial.println("Movement in simulation mode - no servos attached");
  }

  resetAvoidanceState();
}

// Reset all avoidance state
void resetAvoidanceState() {
  avoidState = {0};
}

// Move in a direction without position tracking
void moveDirection(Direction dir, Servo& servo, int pulseValue) {
  if (!simulationMode) {
    servo.writeMicroseconds(pulseValue);
  }

  if (dir == FORWARD) {
    Serial.println("Forward");
  } else if (dir == BACKWARD) {
    Serial.println("Go back");
  } else if (dir == LEFT) {
    Serial.println("Go left");
  } else if (dir == RIGHT) {
    Serial.println("Go right");
  }
}

// Begin or continue a timed avoidance move.
// On the first call startTime must be 0; subsequent calls update duration.
void startTimedMove(Direction dir, Servo& servo, int pulseValue,
                    unsigned long& startTime, unsigned long& duration) {
  if (startTime == 0) {
    startTime = millis();
  }
  unsigned long elapsed = millis() - startTime;
  duration = (elapsed > MAX_AVOID_MS) ? MAX_AVOID_MS : elapsed;

  if (!simulationMode) {
    servo.writeMicroseconds(pulseValue);
  }

  if (dir == FORWARD) {
    Serial.println("Forward (timed)");
  } else if (dir == BACKWARD) {
    Serial.println("Go back (timed)");
  } else if (dir == LEFT) {
    Serial.println("Go left (timed)");
  } else if (dir == RIGHT) {
    Serial.println("Go right (timed)");
  }
}

// Restore position over the same duration spent avoiding.
// startTime must be 0 on the first call (caller resets it at phase transition).
// Returns true while restoring, false when complete (also zeros startTime and duration).
bool doRestorePosition(Direction dir, Servo& servo, int restorePulse,
                       unsigned long& startTime, unsigned long& duration) {
  if (duration == 0) return false;

  if (startTime == 0) {
    startTime = millis();  // mark start of restore phase
  }

  if (millis() - startTime < duration) {
    if (!simulationMode) {
      servo.writeMicroseconds(restorePulse);
    }

    if (dir == FORWARD) {
      Serial.println("Forward (restoring)");
    } else if (dir == BACKWARD) {
      Serial.println("Go back (restoring)");
    } else if (dir == LEFT) {
      Serial.println("Go left (restoring)");
    } else if (dir == RIGHT) {
      Serial.println("Go right (restoring)");
    }
    return true;
  } else {
    if (!simulationMode) {
      servo.writeMicroseconds(NEUTRAL_PULSE);
    }
    startTime = 0;
    duration = 0;
    return false;
  }
}
