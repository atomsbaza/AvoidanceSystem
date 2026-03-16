# Drone Obstacle Avoidance System

## Overview

ระบบ obstacle avoidance สำหรับโดรน/ยานพาหนะ RC ใช้ ultrasonic sensors 4 ทิศทาง (front, back, left, right) เพื่อตรวจจับสิ่งกีดขวางและปรับการเคลื่อนที่โดยอัตโนมัติ รองรับ 3 flight modes และมี interrupt-driven RC signal reading สำหรับ response ที่รวดเร็ว

---

## Features

- **Multi-directional obstacle detection** — ultrasonic sensors 4 ตัว (front, back, left, right)
- **Three flight modes**:
  - **Manual Mode** — หลบสิ่งกีดขวางแบบพื้นฐาน ไม่จำตำแหน่งเดิม
  - **Position Hold Mode** — หลบแล้วกลับตำแหน่งเดิมด้วย time-based tracking
  - **Loiter Mode** — คล้าย Manual แต่ปรับสำหรับการ hover
- **RC signal pass-through** — ส่ง RC signal ผ่านตรงเมื่อไม่มีสิ่งกีดขวาง
- **Interrupt-driven RC reading** — อ่าน PWM signal ผ่าน interrupt สำหรับ timing แม่นยำ
- **Simulation mode** — ทดสอบโดยไม่ต้องมี hardware จริง

---

## Hardware Requirements

- Arduino Mega (หรือ board ที่มี pin เพียงพอ)
- HC-SR04 ultrasonic sensors × 4
- RC receiver อย่างน้อย 3 channels
- Servos หรือ ESCs สำหรับควบคุมยานพาหนะ
- Power supply ที่เหมาะสมกับ setup

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

1. ต่อ hardware ตาม pin configuration ด้านบน
2. ติดตั้ง libraries ที่ต้องการ:
   - `Servo.h` (มาพร้อม Arduino IDE)
   - `NewPing.h` (ติดตั้งผ่าน Arduino Library Manager)
3. **ก่อน upload จริง** — เปลี่ยน `#define SIMULATION_MODE` ใน `config.h` เป็น `false`
4. Upload `avoidance_system.ino` ไปยัง Arduino

---

## Configuration

ค่าที่ปรับได้ทั้งหมดอยู่ใน `config.h`:

| Parameter | Default | Description |
|-----------|---------|-------------|
| `SIMULATION_MODE` | `true` | เปลี่ยนเป็น `false` ก่อน deploy จริง |
| `MAX_DISTANCE` | `50` | ระยะ detect สูงสุด (cm) |
| `FORWARD_PULSE` | `1650` | Servo pulse สำหรับเดินหน้า (µs) |
| `BACKWARD_PULSE` | `1350` | Servo pulse สำหรับถอยหลัง (µs) |
| `NEUTRAL_PULSE` | `1500` | Servo pulse สำหรับหยุดนิ่ง (µs) |
| `MODE_MANUAL_MAX` | `1299` | Channel 5 threshold สำหรับ Manual mode |
| `MODE_POSITION_MIN/MAX` | `1300/1490` | Channel 5 range สำหรับ Position Hold |
| `MODE_LOITER_MIN` | `1750` | Channel 5 threshold สำหรับ Loiter |

---

## Usage

1. เปิด RC transmitter
2. เปิด Arduino และยานพาหนะ
3. เลือก flight mode ด้วย Channel 5:
   - **Manual Mode**: Channel 5 ≤ 1299
   - **Position Hold Mode**: Channel 5 = 1300–1490
   - **Loiter Mode**: Channel 5 ≥ 1750
4. บังคับยานพาหนะตามปกติ — ระบบจะหลบสิ่งกีดขวางอัตโนมัติ

---

## Simulation Mode

ทดสอบระบบโดยไม่ต้องมี hardware จริง:

1. ตรวจสอบว่า `config.h` มี `#define SIMULATION_MODE true`
2. Upload และเปิด Serial Monitor (57600 baud)
3. ระบบจะ cycle ผ่าน 6 scenarios อัตโนมัติทุก 3 วินาที:
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
CLAUDE.md                  # คำแนะนำสำหรับ AI assistant
```

---

## Troubleshooting

| อาการ | สาเหตุที่พบบ่อย |
|-------|----------------|
| เคลื่อนที่ผิดปกติ | ตรวจ sensor wiring และการติดตั้ง |
| ไม่ detect สิ่งกีดขวาง | ตรวจ ultrasonic sensor power และ wiring |
| ไม่ตอบสนอง RC | ตรวจ RC receiver connection และ channel mapping |
| Flight mode ผิด | ตรวจ Channel 5 signal range และ mode thresholds ใน `config.h` |
| ระบบรันใน sim mode บน hardware | ตรวจว่า `SIMULATION_MODE false` ใน `config.h` |

---

## Development Notes

- ดู `PLAN.md` สำหรับ implementation plan ของ v1.1.0 (time-based tracking + avoidance logic fixes)
- ดู `CLAUDE.md` สำหรับ conventions และข้อกำหนดของโปรเจกต์
- อย่าใช้ `delay()` ใน main loop หรือ ISR — ระบบต้องเป็น non-blocking ทั้งหมด
