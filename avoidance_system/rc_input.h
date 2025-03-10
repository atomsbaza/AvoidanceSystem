#ifndef RC_INPUT_H
#define RC_INPUT_H

#include <Arduino.h>
#include "config.h"

// Shared variables updated by ISRs
extern volatile uint32_t bUpdateFlagsShared;
extern volatile uint32_t unChannel1InShared;
extern volatile uint32_t unChannel2InShared;
extern volatile uint32_t unChannel3InShared;
extern volatile uint32_t unChannel4InShared;
extern volatile uint32_t unChannel5InShared;
extern volatile uint32_t unChannel6InShared;
extern volatile uint32_t unChannel7InShared;
extern volatile uint32_t unChannel8InShared;

// Function declarations
void setupRCInput();
void calcChannel3();
void calcChannel4();
void calcChannel5();
FlightMode getCurrentMode(uint32_t channelValue);
void updateLocalRCData();

// Structure to hold RC channel data
struct RCData {
  uint32_t channel1;
  uint32_t channel2;
  uint32_t channel3;
  uint32_t channel4;
  uint32_t channel5;
  uint32_t channel6;
  uint32_t channel7;
  uint32_t channel8;
  uint32_t updateFlags;
};

extern RCData rcData;

#endif // RC_INPUT_H 