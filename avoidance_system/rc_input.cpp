#include "rc_input.h"

// Shared variables updated by the ISR and read by loop
volatile uint32_t bUpdateFlagsShared = 0;
volatile uint32_t unChannel1InShared = 0;
volatile uint32_t unChannel2InShared = 0;
volatile uint32_t unChannel3InShared = 0;
volatile uint32_t unChannel4InShared = 0;
volatile uint32_t unChannel5InShared = 0;
volatile uint32_t unChannel6InShared = 0;
volatile uint32_t unChannel7InShared = 0;
volatile uint32_t unChannel8InShared = 0;

// Local RC data structure
RCData rcData = {0};

// Simulation mode flag (should match the one in sensors.cpp)
extern bool simulationMode;

// Variables for simulated RC input
unsigned long lastRCSimUpdate = 0;
int rcSimScenario = 0;
unsigned long rcSimDuration = 5000; // Duration of each RC simulation scenario in ms

// Setup RC input pins and attach interrupts
void setupRCInput() {
  if (!simulationMode) {
    // Attach interrupts for reading RC channels
    attachInterrupt(digitalPinToInterrupt(CHANNEL3_IN_PIN), calcChannel3, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CHANNEL4_IN_PIN), calcChannel4, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CHANNEL5_IN_PIN), calcChannel5, CHANGE);
  } else {
    // Initialize simulated RC values
    unChannel3InShared = 1500; // Center position
    unChannel4InShared = 1500; // Center position
    unChannel5InShared = 1200; // Manual mode
    
    // Set initial update flags to trigger processing
    bUpdateFlagsShared = CHANNEL3_FLAG | CHANNEL4_FLAG | CHANNEL5_FLAG;
  }
}

// Interrupt service routine for Channel 3
void calcChannel3() {
  static uint32_t ulStart;
  
  if (digitalRead(CHANNEL3_IN_PIN)) {
    ulStart = micros();
  } else {
    unChannel3InShared = (uint32_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL3_FLAG;
  }
}

// Interrupt service routine for Channel 4
void calcChannel4() {
  static uint32_t ulStart;
  
  if (digitalRead(CHANNEL4_IN_PIN)) {
    ulStart = micros();
  } else {
    unChannel4InShared = (uint32_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL4_FLAG;
  }
}

// Interrupt service routine for Channel 5
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

// Update simulated RC values
void updateSimulatedRC() {
  if (millis() - lastRCSimUpdate > rcSimDuration) {
    lastRCSimUpdate = millis();
    
    // Cycle through different RC scenarios
    rcSimScenario = (rcSimScenario + 1) % 5;
    
    // Set up the current scenario
    switch (rcSimScenario) {
      case 0:
        // Center stick positions (neutral)
        unChannel3InShared = 1500;
        unChannel4InShared = 1500;
        Serial.println("RC Sim: Neutral position");
        break;
      case 1:
        // Forward command
        unChannel3InShared = 1700;
        unChannel4InShared = 1500;
        Serial.println("RC Sim: Forward command");
        break;
      case 2:
        // Right command
        unChannel3InShared = 1500;
        unChannel4InShared = 1700;
        Serial.println("RC Sim: Right command");
        break;
      case 3:
        // Change to Position Hold mode
        unChannel5InShared = 1400;
        Serial.println("RC Sim: Switching to Position Hold mode");
        break;
      case 4:
        // Change to Loiter mode
        unChannel5InShared = 1800;
        Serial.println("RC Sim: Switching to Loiter mode");
        break;
    }
    
    // Set update flags to trigger processing
    bUpdateFlagsShared |= CHANNEL3_FLAG | CHANNEL4_FLAG | CHANNEL5_FLAG;
  }
}

// Update local RC data from shared variables
void updateLocalRCData() {
  // In simulation mode, generate RC values
  if (simulationMode) {
    updateSimulatedRC();
  }
  
  // Check if any channels have new signals
  if (bUpdateFlagsShared) {
    noInterrupts(); // Turn interrupts off while taking local copies of shared variables

    // Copy which channels were updated
    rcData.updateFlags = bUpdateFlagsShared;
    
    // Copy channel values when their flags are set
    if (rcData.updateFlags & CHANNEL1_FLAG) {
      rcData.channel1 = unChannel1InShared;
    }
    
    if (rcData.updateFlags & CHANNEL2_FLAG) {
      rcData.channel2 = unChannel2InShared;
    }
    
    if (rcData.updateFlags & CHANNEL3_FLAG) {
      rcData.channel3 = unChannel3InShared;
    }
    
    if (rcData.updateFlags & CHANNEL4_FLAG) {
      rcData.channel4 = unChannel4InShared;
    }
    
    if (rcData.updateFlags & CHANNEL5_FLAG) {
      rcData.channel5 = unChannel5InShared;
    }
    
    if (rcData.updateFlags & CHANNEL6_FLAG) {
      rcData.channel6 = unChannel6InShared;
    }
    
    if (rcData.updateFlags & CHANNEL7_FLAG) {
      rcData.channel7 = unChannel7InShared;
    }
    
    if (rcData.updateFlags & CHANNEL8_FLAG) {
      rcData.channel8 = unChannel8InShared;
    }
    
    // Clear shared copy of updated flags
    bUpdateFlagsShared = 0;
    
    interrupts(); // Turn interrupts back on
  }
} 