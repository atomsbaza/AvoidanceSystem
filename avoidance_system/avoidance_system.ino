#include <Servo.h>
#include <NewPing.h>

// Include our modular components
#include "config.h"
#include "rc_input.h"
#include "sensors.h"
#include "movement.h"
#include "avoidance_logic.h"

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
    // Get current flight mode
    FlightMode currentMode = getCurrentMode(rcData.channel5);
    
    // Handle obstacle avoidance if channel 3 or 4 has updates
    if (rcData.updateFlags & CHANNEL3_FLAG || rcData.updateFlags & CHANNEL4_FLAG) {
      handleObstacleAvoidance(sensorData, currentMode, rcData.channel3, rcData.channel4);
    }
    
    // Always pass through Channel 5 (mode selection)
    if (rcData.updateFlags & CHANNEL5_FLAG) {
      servoChannel5.writeMicroseconds(rcData.channel5);
    }
    
    // Clear update flags after processing
    rcData.updateFlags = 0;
  }
} 