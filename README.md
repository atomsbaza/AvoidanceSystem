# Drone Obstacle Avoidance System

## Overview

An obstacle avoidance system for drones / RC vehicles. Uses four ultrasonic sensors (front, back, left, right) to detect obstacles and adjust movement automatically. Supports three flight modes and uses interrupt-driven RC signal reading for fast response.

---

## Features

- **Multi-directional obstacle detection** — four ultrasonic sensors (front, back, left, right)
- **Three flight modes**:
  - **Manual Mode** — basic obstacle avoidance, no position memory
  - **Position Hold Mode** — avoid then return to the original position via time-based tracking
  - **Loiter Mode** — similar to Manual, tuned for hovering
- **RC signal pass-through** — RC input is forwarded directly when there is no obstacle
- **Interrupt-driven RC reading** — PWM signals read via interrupts for accurate timing
- **Bounded avoidance duration** — single avoidance phase capped by `MAX_AVOID_MS` for safety
- **Sensor fail-safe** — if sensor readings become invalid, the system holds neutral and resets state automatically
- **Simulation mode** — test the system without real hardware

---

## Hardware Requirements

- Arduino Mega (or any board with enough pins)
- HC-SR04 ultrasonic sensors × 4
- RC receiver with at least 3 channels
- Servos or ESCs to drive the vehicle
- Power supply matched to your setup

---

## Pin Configuration

| Component | Pin | Description |
|-----------|-----|-------------|
| RC Input Channel 3 | 3 | Forward/backward control input |
| RC Input Channel 4 | 2 | Left/right control input |
| RC Input Channel 5 | 4 | Flight mode selection input |
| Servo Output Channel 3 | 9 | Forward/backward output |
| Servo Output Channel 4 | 10 | Left/right output |
| Servo Output Channel 5 | 11 | Flight mode output |
| Front Ultrasonic Trigger | 23 | |
| Front Ultrasonic Echo | 22 | |
| Right Ultrasonic Trigger | 25 | |
| Right Ultrasonic Echo | 24 | |
| Left Ultrasonic Trigger | 27 | |
| Left Ultrasonic Echo | 26 | |
| Back Ultrasonic Trigger | 29 | |
| Back Ultrasonic Echo | 28 | |

---

## Installation

1. Wire the hardware according to the pin configuration above.
2. Install the required libraries:
   - `Servo.h` (bundled with the Arduino IDE)
   - `NewPing.h` (install via the Arduino Library Manager)
3. **Before uploading to real hardware**, set `#define SIMULATION_MODE` to `false` in `config.h`.
4. Upload `avoidance_system.ino` to the Arduino.

---

## Configuration

All tunable values live in `config.h`:

| Parameter | Default | Description |
|-----------|---------|-------------|
| `SIMULATION_MODE` | `true` | Set to `false` before deploying to real hardware |
| `MAX_DISTANCE` | `50` | Maximum detection distance (cm) |
| `FORWARD_PULSE` | `1650` | Servo pulse for forward motion (µs) |
| `BACKWARD_PULSE` | `1350` | Servo pulse for backward motion (µs) |
| `NEUTRAL_PULSE` | `1500` | Servo pulse for hold/stop (µs) |
| `MODE_MANUAL_MAX` | `1299` | Channel 5 threshold for Manual mode |
| `MODE_POSITION_MIN/MAX` | `1300/1490` | Channel 5 range for Position Hold |
| `MODE_LOITER_MIN` | `1750` | Channel 5 threshold for Loiter |
| `MAX_AVOID_MS` | `5000` | Maximum duration of a single avoidance phase (ms); restore takes the same amount |
| `SENSOR_INVALID_TIMEOUT_MS` | `1000` | Reset avoidance state after this much continuous invalid sensor time (ms) |

---

## Usage

1. Turn on the RC transmitter.
2. Power up the Arduino and the vehicle.
3. Select a flight mode with Channel 5:
   - **Manual Mode**: Channel 5 ≤ 1299
   - **Position Hold Mode**: Channel 5 = 1300–1490
   - **Loiter Mode**: Channel 5 ≥ 1750
4. Drive the vehicle as usual — the system handles obstacle avoidance automatically.

---

## Simulation Mode

Test the system without real hardware:

1. Make sure `config.h` has `#define SIMULATION_MODE true`.
2. Upload and open the Serial Monitor (57600 baud).
3. The system cycles through 6 scenarios automatically every 3 seconds:
   - No obstacles
   - Obstacle in front
   - Obstacle on right
   - Obstacle on left
   - Obstacle in back
   - Multiple obstacles (front + left)

---

## Project Structure

```
avoidance_system/
├── avoidance_system.ino   # main: setup() + loop()
├── config.h               # constants, pins, thresholds
├── sensors.h / .cpp       # ultrasonic sensor reading
├── movement.h / .cpp      # servo control + time-based position tracking
├── rc_input.h / .cpp      # RC PWM reading via interrupts
└── avoidance_logic.h / .cpp  # avoidance decision logic per flight mode
PLAN.md                    # implementation plan (v1.1.0 refactor)
CLAUDE.md                  # guidance for the AI assistant
```

---

## Troubleshooting

| Symptom | Common cause |
|---------|--------------|
| Erratic movement | Check sensor wiring and mounting |
| Obstacles not detected | Check ultrasonic sensor power and wiring |
| No response to RC | Check RC receiver connection and channel mapping |
| Wrong flight mode | Check Channel 5 signal range and the mode thresholds in `config.h` |
| System runs in sim mode on real hardware | Verify `SIMULATION_MODE false` in `config.h` |
| Restore phase shorter than the avoidance phase | Duration is clamped by `MAX_AVOID_MS`; raise it if you need a longer avoidance window |
| `WARNING: sensors invalid - state reset` appears | Sensor readings stayed invalid longer than `SENSOR_INVALID_TIMEOUT_MS`; check ultrasonic wiring/power |

---

## Development Notes

- See `PLAN.md` for the v1.1.0 implementation plan (time-based tracking + avoidance logic fixes).
- See `CLAUDE.md` for project conventions and constraints.
- Do not use `delay()` in the main loop or in any ISR — the system must remain fully non-blocking.
