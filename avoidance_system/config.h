#ifndef CONFIG_H
#define CONFIG_H

// Constants for pin definitions
// RC Input pins
#define CHANNEL3_IN_PIN 3   // Forward/backward control input pin
#define CHANNEL4_IN_PIN 2   // Left/right control input pin
#define CHANNEL5_IN_PIN 4   // Flight mode selection input pin

// Servo output pins
#define CHANNEL3_OUT_PIN 9  // Forward/backward output pin
#define CHANNEL4_OUT_PIN 10 // Left/right output pin
#define CHANNEL5_OUT_PIN 11 // Flight mode output pin

// Ultrasonic sensor pins
#define FRONT_TRIGGER_PIN 23
#define FRONT_ECHO_PIN 22
#define RIGHT_TRIGGER_PIN 25
#define RIGHT_ECHO_PIN 24
#define LEFT_TRIGGER_PIN 27
#define LEFT_ECHO_PIN 26
#define BACK_TRIGGER_PIN 29
#define BACK_ECHO_PIN 28

// Sensor configuration
#define MAX_DISTANCE 50     // Maximum distance for sensors (in cm)

// RC mode value thresholds
#define MODE_MANUAL_MAX 1299
#define MODE_POSITION_MIN 1300
#define MODE_POSITION_MAX 1490
#define MODE_LOITER_MIN 1750

// Servo pulse values
#define FORWARD_PULSE 1650
#define BACKWARD_PULSE 1350

// Channel update flags
#define CHANNEL1_FLAG 1
#define CHANNEL2_FLAG 2
#define CHANNEL3_FLAG 4
#define CHANNEL4_FLAG 8
#define CHANNEL5_FLAG 16
#define CHANNEL6_FLAG 32
#define CHANNEL7_FLAG 64
#define CHANNEL8_FLAG 128

// Flight mode enum
enum FlightMode {
  MODE_MANUAL,
  MODE_POSITION_HOLD,
  MODE_LOITER
};

// Direction enum
enum Direction {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

#endif // CONFIG_H 