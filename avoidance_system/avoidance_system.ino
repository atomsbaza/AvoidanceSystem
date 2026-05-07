#include <Servo.h>
#include <NewPing.h>

// Include our modular components
#include "config.h"
#include "rc_input.h"
#include "sensors.h"
#include "movement.h"
#include "avoidance_logic.h"

// Track current flight mode so we can detect changes
FlightMode currentMode = MODE_MANUAL;

void setup() {
  Serial.begin(57600);
  Serial.println("Obstacle Avoidance System");

  // Initialize all components
  setupRCInput();
  setupSensors();
  setupMovement();
}

void loop() {
  // Read sensor data
  SensorData sensorData = readSensors();

  // Update RC data from shared variables
  updateLocalRCData();

  // Process RC data if there are updates
  if (rcData.updateFlags) {
    // Get current flight mode and reset avoidance state on mode change
    FlightMode newMode = getCurrentMode(rcData.channel5);
    if (newMode != currentMode) {
      currentMode = newMode;
      resetAvoidanceState();
    }

    // Handle obstacle avoidance when channel 3 or 4 has updates
    if (rcData.updateFlags & CHANNEL3_FLAG || rcData.updateFlags & CHANNEL4_FLAG) {
      static unsigned long invalidSince = 0;
      if (sensorData.isValid) {
        invalidSince = 0;
        handleObstacleAvoidance(sensorData, currentMode, rcData.channel3, rcData.channel4);
      } else {
        // Fail-safe: hold neutral while sensors are invalid
        if (!simulationMode) {
          servoChannel3.writeMicroseconds(NEUTRAL_PULSE);
          servoChannel4.writeMicroseconds(NEUTRAL_PULSE);
        }
        if (invalidSince == 0) invalidSince = millis();
        if (millis() - invalidSince > SENSOR_INVALID_TIMEOUT_MS) {
          resetAvoidanceState();
          Serial.println("WARNING: sensors invalid - state reset");
          invalidSince = millis(); // throttle subsequent logs
        }
      }
    }

    // Pass through Channel 5 (mode selection) — guard against writing in simulation
    if (rcData.updateFlags & CHANNEL5_FLAG) {
      if (!simulationMode) {
        servoChannel5.writeMicroseconds(rcData.channel5);
      }
    }

    // Clear update flags after processing
    rcData.updateFlags = 0;
  }
}
