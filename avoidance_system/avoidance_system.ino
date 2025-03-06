#include "Servo.h"
#include <NewPing.h>

// Constants for pin definitions
#define CHANNEL3_IN_PIN 3   // Channel 1 input pin
#define CHANNEL4_IN_PIN 2   // Channel 2 input pin
#define CHANNEL5_IN_PIN 4   // Channel 5 input pin

#define CHANNEL3_OUT_PIN 9  // Channel 1 output pin
#define CHANNEL4_OUT_PIN 10 // Channel 2 output pin
#define CHANNEL5_OUT_PIN 11 // Channel 5 output pin

// Ultrasonic sensor setup
#define FRONT_TRIGGER_PIN 23
#define FRONT_ECHO_PIN 22
#define RIGHT_TRIGGER_PIN 25
#define RIGHT_ECHO_PIN 24
#define LEFT_TRIGGER_PIN 27
#define LEFT_ECHO_PIN 26
#define BACK_TRIGGER_PIN 29
#define BACK_ECHO_PIN 28
#define MAX_DISTANCE 50     // Maximum distance for sensors (in cm)

// RC mode value thresholds
#define MODE_MANUAL_MAX 1299
#define MODE_POSITION_MIN 1300
#define MODE_POSITION_MAX 1490
#define MODE_LOITER_MIN 1750

// Servo pulse values
#define FORWARD_PULSE 1650
#define BACKWARD_PULSE 1350

// Flight mode constants
enum FlightMode {
  MODE_MANUAL,
  MODE_POSITION_HOLD,
  MODE_LOITER
};

// Direction constants
enum Direction {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

// These bit flags indicate which channels have new signals
#define CHANNEL1_FLAG 1
#define CHANNEL2_FLAG 2
#define CHANNEL3_FLAG 4
#define CHANNEL4_FLAG 8
#define CHANNEL5_FLAG 16
#define CHANNEL6_FLAG 32
#define CHANNEL7_FLAG 64
#define CHANNEL8_FLAG 128

// Servo objects for controlling ESCs and servos
Servo servoChannel3; // Channel 1
Servo servoChannel4; // Channel 2
Servo servoChannel5; // Channel 5

// Ultrasonic sensor objects
NewPing sonarFront(FRONT_TRIGGER_PIN, FRONT_ECHO_PIN, MAX_DISTANCE);
NewPing sonarRight(RIGHT_TRIGGER_PIN, RIGHT_ECHO_PIN, MAX_DISTANCE);
NewPing sonarLeft(LEFT_TRIGGER_PIN, LEFT_ECHO_PIN, MAX_DISTANCE);
NewPing sonarBack(BACK_TRIGGER_PIN, BACK_ECHO_PIN, MAX_DISTANCE);

// Position counters for tracking movement
int forwardCounter = 0;
int backwardCounter = 0;
int rightCounter = 0;
int leftCounter = 0;

// Shared variables updated by the ISR and read by loop
volatile uint32_t bUpdateFlagsShared;
volatile uint32_t unChannel1InShared;
volatile uint32_t unChannel2InShared;
volatile uint32_t unChannel3InShared;
volatile uint32_t unChannel4InShared;
volatile uint32_t unChannel5InShared;
volatile uint32_t unChannel6InShared;
volatile uint32_t unChannel7InShared;
volatile uint32_t unChannel8InShared;

// Function declarations
void moveDirection(Direction dir, Servo& servo, int pulseValue);
void countedMove(Direction dir, Servo& servo, int pulseValue, int& counter);
void restorePosition(Direction dir, Servo& servo, int pulseValue, int& counter);
FlightMode getCurrentMode(uint32_t channelValue);
void handleObstacleAvoidance(int front, int right, int left, int back, FlightMode mode, uint32_t channel3, uint32_t channel4);

void setup() {
  Serial.begin(57600);
  Serial.println("Obstacle Avoidance System");

  // Attach servo objects to output pins
  servoChannel3.attach(CHANNEL3_OUT_PIN);
  servoChannel4.attach(CHANNEL4_OUT_PIN);
  servoChannel5.attach(CHANNEL5_OUT_PIN);

  // Attach interrupts for reading RC channels
  attachInterrupt(digitalPinToInterrupt(CHANNEL3_IN_PIN), calcChannel3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CHANNEL4_IN_PIN), calcChannel4, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CHANNEL5_IN_PIN), calcChannel5, CHANGE);
}

void loop() {
  // Read ultrasonic sensor values
  int frontDistance = sonarFront.ping_cm();
  int rightDistance = sonarRight.ping_cm();
  int leftDistance = sonarLeft.ping_cm();
  int backDistance = sonarBack.ping_cm();

  // Create local variables to hold local copies of the channel inputs
  static uint32_t unChannel1In;
  static uint32_t unChannel2In;
  static uint32_t unChannel3In;
  static uint32_t unChannel4In;
  static uint32_t unChannel5In;
  static uint32_t unChannel6In;
  static uint32_t unChannel7In;
  static uint32_t unChannel8In;
  
  // Local copy of update flags
  static uint32_t bUpdateFlags;

  // Check if any channels have new signals
  if (bUpdateFlagsShared) {
    noInterrupts(); // Turn interrupts off while taking local copies of shared variables

    // Copy which channels were updated
    bUpdateFlags = bUpdateFlagsShared;
    
    // Copy channel values when their flags are set
    if (bUpdateFlags & CHANNEL1_FLAG) {
      unChannel1In = unChannel1InShared;
    }
    
    if (bUpdateFlags & CHANNEL2_FLAG) {
      unChannel2In = unChannel2InShared;
    }
    
    if (bUpdateFlags & CHANNEL3_FLAG) {
      unChannel3In = unChannel3InShared;
    }
    
    if (bUpdateFlags & CHANNEL4_FLAG) {
      unChannel4In = unChannel4InShared;
    }
    
    if (bUpdateFlags & CHANNEL5_FLAG) {
      unChannel5In = unChannel5InShared;
    }
    
    if (bUpdateFlags & CHANNEL6_FLAG) {
      unChannel6In = unChannel6InShared;
    }
    
    if (bUpdateFlags & CHANNEL7_FLAG) {
      unChannel7In = unChannel7InShared;
    }
    
    if (bUpdateFlags & CHANNEL8_FLAG) {
      unChannel8In = unChannel8InShared;
    }
    
    // Clear shared copy of updated flags
    bUpdateFlagsShared = 0;
    
    interrupts(); // Turn interrupts back on
  }
  
  // Process channel updates
  if (bUpdateFlags) {
    // Handle the flight mode and obstacle avoidance
    FlightMode currentMode = getCurrentMode(unChannel5In);
    
    if (bUpdateFlags & CHANNEL3_FLAG || bUpdateFlags & CHANNEL4_FLAG) {
      handleObstacleAvoidance(frontDistance, rightDistance, leftDistance, backDistance,
                              currentMode, unChannel3In, unChannel4In);
    }
    
    // Always pass through Channel 5 (mode selection)
    if (bUpdateFlags & CHANNEL5_FLAG) {
      servoChannel5.writeMicroseconds(unChannel5In);
    }
    
    // Clear update flags after processing
    bUpdateFlags = 0;
  }
}

// Interrupt service routines for RC channels
void calcChannel3() {
  static uint32_t ulStart;
  
  if (digitalRead(CHANNEL3_IN_PIN)) {
    ulStart = micros();
  } else {
    unChannel3InShared = (uint32_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL3_FLAG;
  }
}

void calcChannel4() {
  static uint32_t ulStart;
  
  if (digitalRead(CHANNEL4_IN_PIN)) {
    ulStart = micros();
  } else {
    unChannel4InShared = (uint32_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL4_FLAG;
  }
}

void calcChannel5() {
  static uint32_t ulStart;
  
  if (digitalRead(CHANNEL5_IN_PIN)) {
    ulStart = micros();
  } else {
    unChannel5InShared = (uint32_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL5_FLAG;
  }
}

// Helper function to determine current flight mode based on channel 5 value
FlightMode getCurrentMode(uint32_t channel5Value) {
  if (channel5Value <= MODE_MANUAL_MAX) {
    return MODE_MANUAL;
  } else if (channel5Value >= MODE_POSITION_MIN && channel5Value <= MODE_POSITION_MAX) {
    return MODE_POSITION_HOLD;
  } else if (channel5Value >= MODE_LOITER_MIN) {
    return MODE_LOITER;
  } else {
    // Default to manual mode
    return MODE_MANUAL;
  }
}

// Move in a direction without tracking
void moveDirection(Direction dir, Servo& servo, int pulseValue) {
  servo.writeMicroseconds(pulseValue);
  delay(100);
  
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
  servo.writeMicroseconds(pulseValue);
  delay(100);
  
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

// Restore original position by moving in opposite direction
void restorePosition(Direction dir, Servo& servo, int pulseValue, int& counter) {
  if (counter > 0) {
    counter--;
    servo.writeMicroseconds(pulseValue);
    delay(100);
    
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
}

// Main function to handle obstacle avoidance based on sensor data and flight mode
void handleObstacleAvoidance(int front, int right, int left, int back, 
                            FlightMode mode, uint32_t channel3, uint32_t channel4) {
  
  // Handle Channel 3 (forward/backward movement)
  if (mode == MODE_POSITION_HOLD) {
    // Position hold mode with position memory
    if (front == 0 && back == 0 && forwardCounter == 0 && backwardCounter == 0) {
      servoChannel3.writeMicroseconds(channel3);
    } else if (front != 0 && back == 0) {
      countedMove(BACKWARD, servoChannel3, BACKWARD_PULSE, forwardCounter);
    } else if (front == 0 && forwardCounter != 0) {
      restorePosition(FORWARD, servoChannel3, FORWARD_PULSE, forwardCounter);
    } else if (back != 0 && front == 0) {
      countedMove(FORWARD, servoChannel3, FORWARD_PULSE, backwardCounter);
    } else if (back == 0 && backwardCounter != 0) {
      restorePosition(BACKWARD, servoChannel3, BACKWARD_PULSE, backwardCounter);
    }
  } else {
    // Manual mode or Loiter mode - no position memory
    if (front == 0 && back == 0 && right == 0 && left == 0) {
      servoChannel3.writeMicroseconds(channel3);
    } else if (front != 0 && back == 0) {
      moveDirection(BACKWARD, servoChannel3, BACKWARD_PULSE);
    } else if (back != 0 && front == 0) {
      moveDirection(FORWARD, servoChannel3, FORWARD_PULSE);
    } else if (left != 0 && right != 0 && front != 0 && back == 0) {
      moveDirection(BACKWARD, servoChannel3, BACKWARD_PULSE);
    } else if (left != 0 && right != 0 && back != 0 && front == 0) {
      moveDirection(FORWARD, servoChannel3, FORWARD_PULSE);
    } else if (left != 0 && right != 0 && back == 0 && front == 0) {
      moveDirection(FORWARD, servoChannel3, FORWARD_PULSE);
    } else if ((left != 0 && right == 0) || (left == 0 && right != 0)) {
      if (front != 0 && back == 0) {
        moveDirection(BACKWARD, servoChannel3, BACKWARD_PULSE);
      } else if (back != 0 && front == 0) {
        moveDirection(FORWARD, servoChannel3, FORWARD_PULSE);
      }
    }
  }
  
  // Handle Channel 4 (left/right movement)
  if (mode == MODE_POSITION_HOLD) {
    // Position hold mode with position memory
    if (right == 0 && left == 0 && rightCounter == 0 && leftCounter == 0) {
      servoChannel4.writeMicroseconds(channel4);
    } else if (right != 0 && left == 0) {
      countedMove(LEFT, servoChannel4, BACKWARD_PULSE, rightCounter);
    } else if (right == 0 && rightCounter != 0) {
      restorePosition(RIGHT, servoChannel4, FORWARD_PULSE, rightCounter);
    } else if (left != 0 && right == 0) {
      countedMove(RIGHT, servoChannel4, FORWARD_PULSE, leftCounter);
    } else if (left == 0 && leftCounter != 0) {
      restorePosition(LEFT, servoChannel4, BACKWARD_PULSE, leftCounter);
    }
  } else {
    // Manual mode or Loiter mode - no position memory
    if (right == 0 && left == 0 && back == 0 && front == 0) {
      servoChannel4.writeMicroseconds(channel4);
    } else if (right != 0 && left == 0 && back == 0 && front == 0) {
      moveDirection(LEFT, servoChannel4, BACKWARD_PULSE);
    } else if (left != 0 && right == 0 && back == 0 && front == 0) {
      moveDirection(RIGHT, servoChannel4, FORWARD_PULSE);
    } else if (front != 0 && back != 0) {
      if (left != 0 && right == 0) {
        moveDirection(RIGHT, servoChannel4, FORWARD_PULSE);
      } else if (right != 0 && left == 0) {
        moveDirection(LEFT, servoChannel4, BACKWARD_PULSE);
      } else {
        moveDirection(LEFT, servoChannel4, BACKWARD_PULSE);
      }
    } else if ((left != 0 && right == 0) || (left == 0 && right != 0)) {
      if (left != 0) {
        moveDirection(RIGHT, servoChannel4, FORWARD_PULSE);
      } else {
        moveDirection(LEFT, servoChannel4, BACKWARD_PULSE);
      }
    }
  }
}