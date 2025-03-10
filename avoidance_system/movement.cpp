#include "movement.h"
#include "sensors.h"

// Position counters for tracking movement
int forwardCounter = 0;
int backwardCounter = 0;
int rightCounter = 0;
int leftCounter = 0;

// Servo objects for controlling ESCs and servos
Servo servoChannel3; // Forward/backward control
Servo servoChannel4; // Left/right control
Servo servoChannel5; // Mode control

// Initialize servos and attach them to pins
void setupMovement() {
  if (!simulationMode) {
    // Attach servo objects to output pins
    servoChannel3.attach(CHANNEL3_OUT_PIN);
    servoChannel4.attach(CHANNEL4_OUT_PIN);
    servoChannel5.attach(CHANNEL5_OUT_PIN);
  } else {
    Serial.println("Movement in simulation mode - no servos attached");
  }
  
  // Initialize counters
  resetCounters();
}

// Reset all position counters
void resetCounters() {
  forwardCounter = 0;
  backwardCounter = 0;
  rightCounter = 0;
  leftCounter = 0;
}

// Move in a direction without tracking
void moveDirection(Direction dir, Servo& servo, int pulseValue) {
  if (!simulationMode) {
    servo.writeMicroseconds(pulseValue);
  }
  
  // Non-blocking approach - no delay here
  
  // Debug output
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

// Move in a direction and track the movement count
void countedMove(Direction dir, Servo& servo, int pulseValue, int& counter) {
  counter++;
  
  if (!simulationMode) {
    servo.writeMicroseconds(pulseValue);
  }
  
  // Non-blocking approach - no delay here
  
  // Debug output
  if (dir == FORWARD) {
    Serial.println("Forward (counted)");
  } else if (dir == BACKWARD) {
    Serial.println("Go back (counted)");
  } else if (dir == LEFT) {
    Serial.println("Go left (counted)");
  } else if (dir == RIGHT) {
    Serial.println("Go right (counted)");
  }
}

// Restore original position by moving in opposite direction
void restorePosition(Direction dir, Servo& servo, int pulseValue, int& counter) {
  if (counter > 0) {
    counter--;
    
    if (!simulationMode) {
      servo.writeMicroseconds(pulseValue);
    }
    
    // Non-blocking approach - no delay here
    
    // Debug output
    if (dir == FORWARD) {
      Serial.println("Forward (restoring)");
    } else if (dir == BACKWARD) {
      Serial.println("Go back (restoring)");
    } else if (dir == LEFT) {
      Serial.println("Go left (restoring)");
    } else if (dir == RIGHT) {
      Serial.println("Go right (restoring)");
    }
  }
} 