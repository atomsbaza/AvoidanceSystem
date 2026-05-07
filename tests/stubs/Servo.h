#ifndef SERVO_STUB_H
#define SERVO_STUB_H

class Servo {
public:
  int lastPulse = -1;
  bool attached = false;
  void attach(int) { attached = true; }
  void writeMicroseconds(int us) { lastPulse = us; }
};

#endif
