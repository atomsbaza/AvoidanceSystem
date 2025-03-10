# Drone Obstacle Avoidance System

## Overview
This project implements an intelligent obstacle avoidance system for drones or similar RC vehicles. The system uses ultrasonic sensors to detect obstacles in four directions (front, back, left, right) and automatically adjusts the vehicle's movement to prevent collisions while maintaining the desired flight path as much as possible.

## Features
- **Multi-directional obstacle detection** using four ultrasonic sensors
- **Three flight modes**:
  - **Manual Mode**: Basic obstacle avoidance without position memory
  - **Position Hold Mode**: Advanced obstacle avoidance with position memory (returns to original position after avoiding obstacles)
  - **Loiter Mode**: Similar to manual mode with different avoidance behavior
- **Position memory system** that remembers the original position and returns to it after obstacle avoidance (in Position Hold mode)
- **RC signal pass-through** when no obstacles are detected
- **Interrupt-driven RC signal reading** for precise timing and responsive control

## Hardware Requirements
- Arduino board (Mega or similar with sufficient pins)
- 4× HC-SR04 ultrasonic sensors (or compatible)
- RC receiver with at least 3 channels
- Servos or ESCs for vehicle control
- Drone or RC vehicle compatible with the system
- Power supply appropriate for your setup
- Mounting hardware and cables

## Pin Configuration
| Component | Pin | Description |
|-----------|-----|-------------|
| RC Input Channel 3 | 3 | Forward/backward control |
| RC Input Channel 4 | 2 | Left/right control |
| RC Input Channel 5 | 4 | Flight mode selection |
| Servo Output Channel 3 | 9 | Forward/backward output |
| Servo Output Channel 4 | 10 | Left/right output |
| Servo Output Channel 5 | 11 | Flight mode output |
| Front Ultrasonic Trigger | 23 | Front sensor trigger |
| Front Ultrasonic Echo | 22 | Front sensor echo |
| Right Ultrasonic Trigger | 25 | Right sensor trigger |
| Right Ultrasonic Echo | 24 | Right sensor echo |
| Left Ultrasonic Trigger | 27 | Left sensor trigger |
| Left Ultrasonic Echo | 26 | Left sensor echo |
| Back Ultrasonic Trigger | 29 | Back sensor trigger |
| Back Ultrasonic Echo | 28 | Back sensor echo |

## Installation
1. Connect the hardware according to the pin configuration table above
2. Install the required libraries:
   - Servo.h (included with Arduino IDE)
   - NewPing.h (install via Arduino Library Manager)
3. Upload the `avoidance_system.ino` sketch to your Arduino board

## Usage
1. Power on your RC transmitter
2. Power on the Arduino and the vehicle
3. Select the desired flight mode using Channel 5:
   - Manual Mode: Channel 5 value ≤ 1299
   - Position Hold Mode: Channel 5 value between 1300-1490
   - Loiter Mode: Channel 5 value ≥ 1750
4. Control the vehicle normally using your RC transmitter
5. The system will automatically avoid obstacles while trying to follow your control inputs

## Flight Modes Explained
- **Manual Mode**: The system will avoid obstacles but won't remember the original position. Once an obstacle is cleared, the vehicle will continue in the new direction.
- **Position Hold Mode**: The system remembers how far it moved to avoid an obstacle and will return to the original flight path once the obstacle is cleared.
- **Loiter Mode**: Similar to Manual Mode but with different avoidance behavior optimized for hovering or loitering in place.

## Customization
You can customize the system by modifying these parameters in the code:
- `MAX_DISTANCE`: Maximum detection distance for the ultrasonic sensors (default: 50cm)
- `MODE_MANUAL_MAX`, `MODE_POSITION_MIN`, etc.: Thresholds for flight mode selection
- `FORWARD_PULSE`, `BACKWARD_PULSE`: Servo pulse values for movement directions

## Troubleshooting
- **Erratic movement**: Check sensor connections and ensure they're properly mounted
- **No obstacle detection**: Verify ultrasonic sensor wiring and power
- **System not responding to RC**: Check RC receiver connections and channel mapping
- **Incorrect flight mode**: Verify Channel 5 signal range and mode thresholds
