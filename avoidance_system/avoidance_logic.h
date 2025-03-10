#ifndef AVOIDANCE_LOGIC_H
#define AVOIDANCE_LOGIC_H

#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "movement.h"
#include "rc_input.h"

// Function declarations
void handleObstacleAvoidance(const SensorData& sensorData, FlightMode mode, uint32_t channel3, uint32_t channel4);
void handleManualModeAvoidance(const SensorData& sensorData, uint32_t channel3, uint32_t channel4);
void handlePositionHoldAvoidance(const SensorData& sensorData, uint32_t channel3, uint32_t channel4);
void handleLoiterModeAvoidance(const SensorData& sensorData, uint32_t channel3, uint32_t channel4);

#endif // AVOIDANCE_LOGIC_H 