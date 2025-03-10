#include "sensors.h"

// Ultrasonic sensor objects
NewPing sonarFront(FRONT_TRIGGER_PIN, FRONT_ECHO_PIN, MAX_DISTANCE);
NewPing sonarRight(RIGHT_TRIGGER_PIN, RIGHT_ECHO_PIN, MAX_DISTANCE);
NewPing sonarLeft(LEFT_TRIGGER_PIN, LEFT_ECHO_PIN, MAX_DISTANCE);
NewPing sonarBack(BACK_TRIGGER_PIN, BACK_ECHO_PIN, MAX_DISTANCE);

// Flag to enable simulation mode (no hardware needed)
bool simulationMode = true;

// Simulated obstacle scenario (0 = no obstacle, >0 = distance to obstacle)
int simulatedFront = 0;
int simulatedRight = 0;
int simulatedLeft = 0;
int simulatedBack = 0;
unsigned long lastSimUpdate = 0;
int simulationScenario = 0;

// Initialize sensors
void setupSensors() {
  // No specific setup needed for NewPing library
  // Pins are configured when the NewPing objects are created
  
  if (simulationMode) {
    Serial.println("RUNNING IN SIMULATION MODE - NO HARDWARE REQUIRED");
  }
}

// Read all sensor values and return a SensorData structure
SensorData readSensors() {
  SensorData data;
  
  if (!simulationMode) {
    // Read actual distances from sensors
    data.frontDistance = sonarFront.ping_cm();
    data.rightDistance = sonarRight.ping_cm();
    data.leftDistance = sonarLeft.ping_cm();
    data.backDistance = sonarBack.ping_cm();
  } else {
    // Generate simulated sensor readings
    updateSimulation();
    
    // Use simulated values
    data.frontDistance = simulatedFront;
    data.rightDistance = simulatedRight;
    data.leftDistance = simulatedLeft;
    data.backDistance = simulatedBack;
    
    // Print simulated sensor values for debugging
    Serial.print("Simulated sensors - F:");
    Serial.print(data.frontDistance);
    Serial.print(" R:");
    Serial.print(data.rightDistance);
    Serial.print(" L:");
    Serial.print(data.leftDistance);
    Serial.print(" B:");
    Serial.println(data.backDistance);
  }
  
  // Validate readings
  data.isValid = true;
  
  return data;
}

// Update simulation values to create test scenarios
void updateSimulation() {
  // Update simulation every 3 seconds
  if (millis() - lastSimUpdate > 3000) {
    lastSimUpdate = millis();
    
    // Cycle through different test scenarios
    simulationScenario = (simulationScenario + 1) % 6;
    
    // Reset all simulated values
    simulatedFront = 0;
    simulatedRight = 0;
    simulatedLeft = 0;
    simulatedBack = 0;
    
    // Set up the current scenario
    switch (simulationScenario) {
      case 0:
        // No obstacles
        Serial.println("Simulation: No obstacles");
        break;
      case 1:
        // Obstacle in front
        simulatedFront = 30;
        Serial.println("Simulation: Obstacle in front");
        break;
      case 2:
        // Obstacle on right
        simulatedRight = 25;
        Serial.println("Simulation: Obstacle on right");
        break;
      case 3:
        // Obstacle on left
        simulatedLeft = 20;
        Serial.println("Simulation: Obstacle on left");
        break;
      case 4:
        // Obstacle in back
        simulatedBack = 35;
        Serial.println("Simulation: Obstacle in back");
        break;
      case 5:
        // Multiple obstacles
        simulatedFront = 40;
        simulatedLeft = 15;
        Serial.println("Simulation: Multiple obstacles (front and left)");
        break;
    }
  }
}

// Validate a single sensor reading
bool validateSensorReading(int reading) {
  // For simulation mode, all readings are considered valid
  if (simulationMode) {
    return true;
  }
  
  // For real hardware, implement validation logic
  return true;
} 