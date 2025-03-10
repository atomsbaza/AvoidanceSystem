#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <NewPing.h>
#include "config.h"

// Sensor data structure
struct SensorData {
  int frontDistance;
  int rightDistance;
  int leftDistance;
  int backDistance;
  bool isValid;
};

// Function declarations
void setupSensors();
SensorData readSensors();
bool validateSensorReading(int reading);
void updateSimulation();

// External sensor objects
extern NewPing sonarFront;
extern NewPing sonarRight;
extern NewPing sonarLeft;
extern NewPing sonarBack;

// Simulation mode flag
extern bool simulationMode;

#endif // SENSORS_H 