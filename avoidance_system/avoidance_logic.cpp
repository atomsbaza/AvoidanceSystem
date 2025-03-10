#include "avoidance_logic.h"

// Main function to handle obstacle avoidance based on sensor data and flight mode
void handleObstacleAvoidance(const SensorData& sensorData, FlightMode mode, uint32_t channel3, uint32_t channel4) {
  // Dispatch to the appropriate handler based on flight mode
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
  
  // Handle Channel 3 (forward/backward movement)
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
  
  // Handle Channel 4 (left/right movement)
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

// Handle avoidance in Position Hold mode (with position memory)
void handlePositionHoldAvoidance(const SensorData& sensorData, uint32_t channel3, uint32_t channel4) {
  int front = sensorData.frontDistance;
  int back = sensorData.backDistance;
  int right = sensorData.rightDistance;
  int left = sensorData.leftDistance;
  
  // Handle Channel 3 (forward/backward movement)
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
  
  // Handle Channel 4 (left/right movement)
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
}

// Handle avoidance in Loiter mode (similar to manual but with different behavior)
void handleLoiterModeAvoidance(const SensorData& sensorData, uint32_t channel3, uint32_t channel4) {
  // For now, Loiter mode uses the same logic as Manual mode
  // In a more sophisticated implementation, this could have different behavior
  handleManualModeAvoidance(sensorData, channel3, channel4);
} 