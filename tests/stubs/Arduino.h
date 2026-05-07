#ifndef ARDUINO_STUB_H
#define ARDUINO_STUB_H

#include <cstdint>
#include <iostream>

typedef uint8_t byte;

extern unsigned long mockMillis;
inline unsigned long millis() { return mockMillis; }
inline unsigned long micros() { return mockMillis * 1000UL; }

class SerialStub {
public:
  void begin(unsigned long) {}
  template <typename T> void print(const T& v) { std::cout << v; }
  template <typename T> void println(const T& v) { std::cout << v << "\n"; }
  void println() { std::cout << "\n"; }
};
extern SerialStub Serial;

inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
inline int digitalRead(int) { return 0; }
inline void attachInterrupt(int, void (*)(), int) {}
inline int digitalPinToInterrupt(int p) { return p; }
inline void noInterrupts() {}
inline void interrupts() {}

#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define CHANGE 3
#define RISING 4
#define FALLING 5

#endif
