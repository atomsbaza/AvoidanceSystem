# CLAUDE.md — AvoidanceSystem

คำแนะนำสำหรับ Claude เมื่อทำงานใน repo นี้

---

## โปรเจกต์นี้คืออะไร

Arduino obstacle avoidance system สำหรับโดรน/ยานพาหนะ RC ใช้ ultrasonic sensors 4 ทิศทาง (front, back, left, right) และรับ RC signal ผ่าน interrupt-driven PWM reading

---

## โครงสร้างไฟล์

```
avoidance_system/
├── avoidance_system.ino   # main loop: setup(), loop()
├── config.h               # ค่า constants ทั้งหมด (pins, thresholds, flags)
├── sensors.h / .cpp       # ultrasonic sensor reading + simulation
├── movement.h / .cpp      # servo control + position tracking
├── rc_input.h / .cpp      # RC PWM reading via interrupts (ISR)
├── avoidance_logic.h / .cpp # avoidance decision logic แยกตาม flight mode
└── README.md
PLAN.md                    # implementation plan ปัจจุบัน
CLAUDE.md                  # ไฟล์นี้
README.md                  # root-level README
```

---

## Conventions ที่ต้องรู้

### Hardware
- ใช้ **Arduino Mega** (pin 22-29 สำหรับ ultrasonic, pin 2-4 สำหรับ RC input interrupt)
- RC input อ่านผ่าน `attachInterrupt()` — ISR ต้องสั้นและเร็ว
- Servo output ใช้ `writeMicroseconds()` — range ปกติ 1000-2000 µs, neutral = 1500
- `FORWARD_PULSE = 1650`, `BACKWARD_PULSE = 1350`, `NEUTRAL_PULSE = 1500`

### Simulation Mode
- เปิด/ปิดได้ที่ `#define SIMULATION_MODE` ใน `config.h`
- `true` = ไม่ต้องมี hardware จริง, ใช้ simulated sensor/RC values
- `false` = deploy จริง (ต้องต่อ hardware ก่อน upload)
- **ต้องไม่ hardcode** `simulationMode = true/false` ใน source ไฟล์อื่น

### ISR Safety
- shared variables ระหว่าง ISR กับ main loop ต้องเป็น `volatile`
- ต้องใช้ `noInterrupts()` / `interrupts()` เมื่ออ่าน shared variables ใน loop

### Time-based Movement (ตาม PLAN.md)
- ใช้ `millis()` แทนการนับ loop iteration สำหรับ position restore
- `startTimedMove()` บันทึก timestamp ตอนเริ่มหลบ
- `doRestorePosition()` คืนตำแหน่งด้วยเวลาเท่ากับที่หลบไป

---

## สิ่งที่ห้ามทำ

- อย่า hardcode `simulationMode = true` ใน `.cpp` — ใช้ `SIMULATION_MODE` define จาก `config.h` เท่านั้น
- อย่าใช้ `delay()` ใน loop หรือ ISR — ระบบต้อง non-blocking
- อย่าเพิ่ม logic ใน ISR (`calcChannel3/4/5`) — ให้ทำแค่ timestamp + flag set
- อย่าเขียน `servo.writeMicroseconds()` ในโหมด simulation โดยไม่ check `simulationMode` ก่อน
- อย่า handle avoidance โดยไม่ check `sensorData.isValid` ก่อน

---

## แผนงานปัจจุบัน

ดูรายละเอียดที่ `PLAN.md` — มี 6 tasks ที่ต้องทำสำหรับ version 1.1.0:
1. Time-based position tracking (CRITICAL)
2. Avoidance logic edge cases (CRITICAL)
3. simulationMode compile flag (HIGH)
4. validateSensorReading จริง (MEDIUM)
5. servoChannel5 simulation guard (MEDIUM)
6. updateSimulatedRC header declaration (LOW)

---

## การ verify งาน

หลังแก้ code ทุกครั้ง:
1. Build ด้วย Arduino IDE — ต้องไม่มี compile error
2. รันใน simulation mode — ดู Serial Monitor ว่า avoidance response ครบทุก scenario
3. ตรวจสอบว่า `SIMULATION_MODE false` build ได้โดยไม่ error
