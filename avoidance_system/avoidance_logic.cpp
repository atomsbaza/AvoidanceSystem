#include "avoidance_logic.h"

// Main function to handle obstacle avoidance based on sensor data and flight mode
void handleObstacleAvoidance(const SensorData& sensorData, FlightMode mode, uint32_t channel3, uint32_t channel4) {
  switch (mode) {
    case MODE_POSITION_HOLD:
      handlePositionHoldAvoidance(sensorData, channel3, channel4);
      break;
    case MODE_LOITER:
      handleLoiterModeAvoidance(sensorData, channel3, channel4);
      break;
    case MODE_MANUAL:
    default:
      handleManualModeAvoidance(sensorData, channel3, channel4);
      break;
  }
}

// Handle avoidance in Manual mode (no position memory)
void handleManualModeAvoidance(const SensorData& sensorData, uint32_t channel3, uint32_t channel4) {
  int front = sensorData.frontDistance;
  int back = sensorData.backDistance;
  int right = sensorData.rightDistance;
  int left = sensorData.leftDistance;

  // All directions blocked — stop and warn
  if (front != 0 && back != 0 && left != 0 && right != 0) {
    servoChannel3.writeMicroseconds(NEUTRAL_PULSE);
    servoChannel4.writeMicroseconds(NEUTRAL_PULSE);
    Serial.println("WARNING: All directions blocked!");
    return;
  }

  // Handle Channel 3 (forward/backward movement)
  if (front == 0 && back == 0 && right == 0 && left == 0) {
    servoChannel3.writeMicroseconds(channel3);
  } else if (front != 0 && back == 0) {
    moveDirection(BACKWARD, servoChannel3, BACKWARD_PULSE);
  } else if (back != 0 && front == 0) {
    moveDirection(FORWARD, servoChannel3, FORWARD_PULSE);
  } else if ((left != 0) ^ (right != 0)) {
    // Exactly one lateral side blocked; front+back equal status here
    if (front == 0 && back == 0) {
      servoChannel3.writeMicroseconds(channel3); // pass-through; lateral handled by ch4
    } else {
      servoChannel3.writeMicroseconds(NEUTRAL_PULSE); // front+back both blocked
    }
  }

  // Handle Channel 4 (left/right movement)
  if (right == 0 && left == 0 && back == 0 && front == 0) {
    servoChannel4.writeMicroseconds(channel4);
  } else if (front != 0 && back != 0) {
    if (left != 0 && right == 0) {
      moveDirection(RIGHT, servoChannel4, FORWARD_PULSE);
    } else if (right != 0 && left == 0) {
      moveDirection(LEFT, servoChannel4, BACKWARD_PULSE);
    } else {
      servoChannel4.writeMicroseconds(NEUTRAL_PULSE);
    }
  } else if (left != 0 && right == 0) {
    moveDirection(RIGHT, servoChannel4, FORWARD_PULSE);
  } else if (right != 0 && left == 0) {
    moveDirection(LEFT, servoChannel4, BACKWARD_PULSE);
  }
}

// Handle avoidance in Position Hold mode (with time-based position memory)
void handlePositionHoldAvoidance(const SensorData& sensorData, uint32_t channel3, uint32_t channel4) {
  int front = sensorData.frontDistance;
  int back = sensorData.backDistance;
  int right = sensorData.rightDistance;
  int left = sensorData.leftDistance;

  // All directions blocked — stop and warn
  if (front != 0 && back != 0 && left != 0 && right != 0) {
    servoChannel3.writeMicroseconds(NEUTRAL_PULSE);
    servoChannel4.writeMicroseconds(NEUTRAL_PULSE);
    Serial.println("WARNING: All directions blocked!");
    return;
  }

  // Handle Channel 3 (forward/backward movement)
  if (front == 0 && back == 0 && avoidState.forwardDuration == 0 && avoidState.backwardDuration == 0) {
    servoChannel3.writeMicroseconds(channel3);
  } else if (front != 0 && back == 0) {
    if (!avoidState.forwardAvoidActive) {
      avoidState.forwardStartTime = 0;  // reset for fresh avoidance timing
      avoidState.forwardAvoidActive = true;
    }
    startTimedMove(BACKWARD, servoChannel3, BACKWARD_PULSE,
                   avoidState.forwardStartTime, avoidState.forwardDuration);
  } else if (front == 0 && avoidState.forwardDuration > 0) {
    if (avoidState.forwardAvoidActive) {
      avoidState.forwardStartTime = 0;  // reset for restore timing
      avoidState.forwardAvoidActive = false;
    }
    doRestorePosition(FORWARD, servoChannel3, FORWARD_PULSE,
                      avoidState.forwardStartTime, avoidState.forwardDuration);
  } else if (back != 0 && front == 0) {
    if (!avoidState.backwardAvoidActive) {
      avoidState.backwardStartTime = 0;
      avoidState.backwardAvoidActive = true;
    }
    startTimedMove(FORWARD, servoChannel3, FORWARD_PULSE,
                   avoidState.backwardStartTime, avoidState.backwardDuration);
  } else if (back == 0 && avoidState.backwardDuration > 0) {
    if (avoidState.backwardAvoidActive) {
      avoidState.backwardStartTime = 0;
      avoidState.backwardAvoidActive = false;
    }
    doRestorePosition(BACKWARD, servoChannel3, BACKWARD_PULSE,
                      avoidState.backwardStartTime, avoidState.backwardDuration);
  }

  // Handle Channel 4 (left/right movement)
  if (right == 0 && left == 0 && avoidState.rightDuration == 0 && avoidState.leftDuration == 0) {
    servoChannel4.writeMicroseconds(channel4);
  } else if (right != 0 && left == 0) {
    if (!avoidState.rightAvoidActive) {
      avoidState.rightStartTime = 0;
      avoidState.rightAvoidActive = true;
    }
    startTimedMove(LEFT, servoChannel4, BACKWARD_PULSE,
                   avoidState.rightStartTime, avoidState.rightDuration);
  } else if (right == 0 && avoidState.rightDuration > 0) {
    if (avoidState.rightAvoidActive) {
      avoidState.rightStartTime = 0;
      avoidState.rightAvoidActive = false;
    }
    doRestorePosition(RIGHT, servoChannel4, FORWARD_PULSE,
                      avoidState.rightStartTime, avoidState.rightDuration);
  } else if (left != 0 && right == 0) {
    if (!avoidState.leftAvoidActive) {
      avoidState.leftStartTime = 0;
      avoidState.leftAvoidActive = true;
    }
    startTimedMove(RIGHT, servoChannel4, FORWARD_PULSE,
                   avoidState.leftStartTime, avoidState.leftDuration);
  } else if (left == 0 && avoidState.leftDuration > 0) {
    if (avoidState.leftAvoidActive) {
      avoidState.leftStartTime = 0;
      avoidState.leftAvoidActive = false;
    }
    doRestorePosition(LEFT, servoChannel4, BACKWARD_PULSE,
                      avoidState.leftStartTime, avoidState.leftDuration);
  }
}

// Handle avoidance in Loiter mode (same logic as Manual mode)
void handleLoiterModeAvoidance(const SensorData& sensorData, uint32_t channel3, uint32_t channel4) {
  handleManualModeAvoidance(sensorData, channel3, channel4);
}
