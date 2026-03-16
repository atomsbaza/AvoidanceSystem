# Refactor + Bug Fix Plan — AvoidanceSystem v1.1.0

## Context

ระบบ obstacle avoidance สำหรับโดรน/ยานพาหนะ RC มีปัญหาหลักหลายจุดที่ทำให้ไม่สามารถใช้งานจริงได้อย่างน่าเชื่อถือ:
- Counter-based position tracking ไม่ accurate เพราะ loop rate ไม่คงที่
- Missing edge cases ในการ avoidance logic (all-blocked, dead code, arbitrary fallback)
- `simulationMode` hardcode ไว้ใน source code ทำให้ต้องแก้ไฟล์ก่อน deploy จริง
- `validateSensorReading()` ไม่ทำงานจริง

---

## สิ่งที่ต้องแก้ (เรียงลำดับ priority)

### 1. [CRITICAL] เปลี่ยน counter-based เป็น time-based tracking

**ไฟล์:** `movement.h`, `movement.cpp`

**ปัญหา:** `countedMove()` นับ loop iteration แทนเวลาจริง ทำให้ restore ไม่ตรงตำแหน่ง เพราะ loop rate ขึ้นอยู่กับ Serial I/O, sensor ping delay, และ interrupt timing

**วิธีแก้:**
- ลบ `int forwardCounter/backwardCounter/rightCounter/leftCounter` และ `resetCounters()`
- เพิ่ม `AvoidanceState` struct ที่เก็บ timestamp และ duration ของแต่ละ direction
- เปลี่ยน `countedMove()` → `startTimedMove()` — บันทึก `millis()` ตอนเริ่มหลบ
- เปลี่ยน `restorePosition()` → `doRestorePosition()` — คืนตำแหน่งด้วยเวลาเท่ากับที่หลบไป
- เพิ่ม `NEUTRAL_PULSE 1500` ใน `config.h`

```cpp
// movement.h — โครงสร้างใหม่
struct AvoidanceState {
  unsigned long forwardStartTime;   // millis() ตอนเริ่มหลบไปข้างหลัง
  unsigned long backwardStartTime;  // millis() ตอนเริ่มหลบไปข้างหน้า
  unsigned long rightStartTime;     // millis() ตอนเริ่มหลบไปซ้าย
  unsigned long leftStartTime;      // millis() ตอนเริ่มหลบไปขวา
  unsigned long forwardDuration;    // ms ที่หลบไปแล้ว (สะสม)
  unsigned long backwardDuration;
  unsigned long rightDuration;
  unsigned long leftDuration;
};
extern AvoidanceState avoidState;

void startTimedMove(Direction dir, Servo& servo, int pulseValue,
                    unsigned long& startTime, unsigned long& duration);
bool doRestorePosition(Direction dir, Servo& servo, int restorePulse,
                       unsigned long& startTime, unsigned long duration);
void resetAvoidanceState();
```

---

### 2. [CRITICAL] แก้ missing edge cases ใน avoidance logic

**ไฟล์:** `avoidance_logic.cpp`

#### 2a. กรณีถูกล้อมทุกทิศ (all 4 directions blocked)
```cpp
// เพิ่มเป็น early-return ตัวแรกใน handleManualModeAvoidance() และ handlePositionHoldAvoidance()
if (front != 0 && back != 0 && left != 0 && right != 0) {
  servoChannel3.writeMicroseconds(NEUTRAL_PULSE);
  servoChannel4.writeMicroseconds(NEUTRAL_PULSE);
  Serial.println("WARNING: All directions blocked!");
  return;
}
```

#### 2b. แก้ arbitrary fallback บน channel 4 (line ~61)
กรณี `front != 0 && back != 0 && left == 0 && right == 0`:
```cpp
} else {
  servoChannel4.writeMicroseconds(NEUTRAL_PULSE); // หยุดรอ RC แทน move left แบบ arbitrary
}
```

#### 2c. แก้ dead code บน channel 3 — กรณี only one lateral side blocked แต่ front+back ทั้งคู่ blocked
```cpp
} else if ((left != 0 && right == 0) || (left == 0 && right != 0)) {
  if (front != 0 && back == 0) {
    moveDirection(BACKWARD, servoChannel3, BACKWARD_PULSE);
  } else if (back != 0 && front == 0) {
    moveDirection(FORWARD, servoChannel3, FORWARD_PULSE);
  } else {
    // front+back ทั้งคู่ blocked — หยุดนิ่ง
    servoChannel3.writeMicroseconds(NEUTRAL_PULSE);
  }
}
```

#### 2d. Reset avoidance state เมื่อ mode เปลี่ยน
เรียก `resetAvoidanceState()` ใน `avoidance_system.ino` เมื่อ `currentMode` เปลี่ยนค่า

---

### 3. [HIGH] ย้าย simulationMode เป็น compile-time flag

**ไฟล์:** `config.h`, `sensors.cpp`

```cpp
// config.h — เพิ่ม
// *** เปลี่ยนเป็น false ก่อน upload จริง ***
#define SIMULATION_MODE true

// sensors.cpp — แก้บรรทัด 10
bool simulationMode = SIMULATION_MODE;
```

---

### 4. [MEDIUM] Implement validateSensorReading() จริง

**ไฟล์:** `sensors.cpp`, `avoidance_system.ino`

```cpp
// sensors.cpp
bool validateSensorReading(int reading) {
  if (reading == 0) return true;           // 0 = no obstacle (valid)
  return (reading >= 2 && reading <= MAX_DISTANCE);
}

// readSensors() — เพิ่มหลัง read ทุกตัว
data.isValid = validateSensorReading(data.frontDistance) &&
               validateSensorReading(data.rightDistance) &&
               validateSensorReading(data.leftDistance) &&
               validateSensorReading(data.backDistance);
```

```cpp
// avoidance_system.ino — เพิ่ม guard ก่อน handleObstacleAvoidance()
if (sensorData.isValid) {
  handleObstacleAvoidance(sensorData, currentMode, rcData.channel3, rcData.channel4);
}
```

---

### 5. [MEDIUM] Guard servoChannel5 write ด้วย simulation check

**ไฟล์:** `avoidance_system.ino:40`

```cpp
if (rcData.updateFlags & CHANNEL5_FLAG) {
  if (!simulationMode) {
    servoChannel5.writeMicroseconds(rcData.channel5);
  }
}
```

---

### 6. [LOW] เพิ่ม updateSimulatedRC declaration ใน header

**ไฟล์:** `rc_input.h`

```cpp
void updateSimulatedRC(); // เพิ่มใน function declarations section
```

---

## ไฟล์ที่ต้องแก้

| ไฟล์ | การเปลี่ยนแปลง |
|------|----------------|
| `avoidance_system/config.h` | เพิ่ม `SIMULATION_MODE` define + `NEUTRAL_PULSE 1500` |
| `avoidance_system/movement.h` | แทน counter ด้วย `AvoidanceState` struct + prototype ใหม่ |
| `avoidance_system/movement.cpp` | แทน `countedMove`/`restorePosition` ด้วย time-based functions |
| `avoidance_system/avoidance_logic.cpp` | แก้ทุก edge case ที่ขาดหาย |
| `avoidance_system/avoidance_system.ino` | เพิ่ม `isValid` check + simulation guard บน ch5 + mode-change reset |
| `avoidance_system/sensors.cpp` | implement `validateSensorReading()` + ย้าย simulationMode init |
| `avoidance_system/rc_input.h` | เพิ่ม `updateSimulatedRC()` declaration |

---

## Verification

1. Build ด้วย Arduino IDE — ต้องไม่มี compile error/warning
2. รันใน simulation mode (`SIMULATION_MODE true`) — Serial Monitor ต้องแสดง avoidance response ครบทุก 6 scenarios
3. Scenario "Obstacle in front" → ต้องเห็น `Go back` แล้วตามด้วย `Forward (restoring)` ด้วยระยะเวลาเท่ากัน
4. Scenario "Multiple obstacles (front and left)" → ต้องเห็น ch3 + ch4 response พร้อมกัน
5. เปลี่ยน `SIMULATION_MODE false` แล้ว build → ต้องไม่มี error

---

## Status

- [ ] 1. Time-based tracking (`movement.h`, `movement.cpp`)
- [ ] 2. Avoidance logic edge cases (`avoidance_logic.cpp`)
- [ ] 3. simulationMode compile flag (`config.h`, `sensors.cpp`)
- [ ] 4. validateSensorReading (`sensors.cpp`, `avoidance_system.ino`)
- [ ] 5. servoChannel5 simulation guard (`avoidance_system.ino`)
- [ ] 6. updateSimulatedRC header declaration (`rc_input.h`)
